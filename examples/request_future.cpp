#include <ozo/connection_info.h>
#include <ozo/request.h>
#include <ozo/shortcuts.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/use_future.hpp>

#include <iostream>
#include <thread>

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

    // All IO is asynchronous, therefore we need to define a CompletionToken. We use boost::asio::use_future.
    // To make this example more real all asynchronous operation will be performed in a separate thread.
    // Work guard will help to keep io_context running until we decide to stop it.
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> guard = boost::asio::make_work_guard(io);
    std::thread worker([&io] {
        io.run();
    });

    // Request result is always a set of rows. Client should take care of output object lifetime.
    ozo::rows_of<int> result;

    // This allows to use _SQL literals
    using namespace ozo::literals;
    using namespace std::chrono_literals;
    auto connection = ozo::request(conn_info[io], "SELECT 1"_SQL, 1s, ozo::into(result), asio::use_future);

    // When request is completed we check whether there is an exception. This example should not produce any errors
    // if there are no problems with target database, network or permissions for given user in the connection
    // string.
    try {
        // Here we wait until asynchronous operation in a different thread is finished. If any error is occured
        // an exception will be thrown. Boost only provides static error code message without any additional context.
        connection.get();

        // Just print request result
        std::cout << "Selected:" << std::endl;
        for (auto value : result) {
            std::cout << std::get<0>(value) << std::endl;
        }
    } catch (const std::exception& error) {
        std::cout << "Request failed with error: " << error.what();
    }

    // Reset work guard to release io and make it able to stop.
    guard.reset();

    // Make sure thread is finished.
    worker.join();

    return 0;
}
