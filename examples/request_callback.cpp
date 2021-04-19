#include <ozo/connection_info.h>
#include <ozo/request.h>
#include <ozo/shortcuts.h>

#include <boost/asio/io_service.hpp>

#include <iostream>

namespace asio = boost::asio;

int main(int argc, char **argv) {
    std::cout << "OZO request example" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <connection string>\n";
        return 1;
    }

    // Ozo perform all IO using Boost.Asio, so first thing we need to do is setup asio::io_context
    asio::io_context io;

    // To make a request we need to make a ConnectionSource. It knows how to connect to database using
    // connection string. See https://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-CONNSTRING
    // how to make a connection string.
    auto conn_info = ozo::connection_info(argv[1]);

    // Request result is always set of rows. Client should take care of output object lifetime.
    auto result = std::make_unique<ozo::rows_of<int>>();

    // First we take a reference to result to be able move unique_ptr to callback context.
    const auto result_ref = ozo::into(*result);

    // All IO is asynchronous, therefore we need to define a CompletionToken.
    // We use callback function that will be called after operation is finished.
    auto callback = [result = std::move(result)] (ozo::error_code ec, auto connection) {
        // When request is completed we check whether there is an error. This example should not produce any errors
        // if there are no problems with target database, network or permissions for given user in connection
        // string.
        if (ec) {
            std::cout << "Request failed with error: " << ec.message();
            // Here we should check if the connection is in null state to avoid UB.
            if (!ozo::is_null_recursive(connection)) {
                // Then check for libpq native error message and print if it's there
                if (auto msg = ozo::error_message(connection); !msg.empty()) {
                    std::cout << ", error message: " << msg;
                }
                // Sometimes libpq native error message is not enough, so let's check
                // the additional error context from OZO
                if (auto ctx = ozo::get_error_context(connection); !ctx.empty()) {
                    std::cout << ", error context: " << ctx;
                }
            }
            std::cout << std::endl;
            return;
        }

        // Just print request result
        std::cout << "Selected:" << std::endl;
        for (auto value : *result) {
            std::cout << std::get<0>(value) << std::endl;
        }
    };

    // This allows to use _SQL literals
    using namespace ozo::literals;
    using namespace std::chrono_literals;
    ozo::request(conn_info[io], "SELECT 1"_SQL, 1s, result_ref, asio::bind_executor(io, std::move(callback)));

    io.run();

    return 0;
}
