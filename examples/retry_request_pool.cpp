#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>
#include <ozo/request.h>
#include <ozo/shortcuts.h>
#include <ozo/failover/retry.h>

#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>

#include <iostream>

namespace asio = boost::asio;

template <class Conn>
void print_error(ozo::error_code ec, const Conn& conn) {
    std::cout << "error code message: \"" << ec.message();
    // Here we should check if the connection is in null state to avoid UB.
    if (!ozo::is_null_recursive(conn)) {
        std::cout << "\", libpq error message: \"" << ozo::error_message(conn)
            << "\", error context: \"" << ozo::get_error_context(conn);
    }
    std::cout << "\"" << std::endl;
}

const auto retry_error = [](ozo::error_code ec, const auto& conn) {
    std::cout << "Retry failed; ";
    print_error(ec, conn);
};

int main(int argc, char **argv) {
    using namespace std::chrono_literals;

    std::cout << "OZO retry request with connection pool example" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <connection string>\n";
        return 1;
    }

    asio::io_context io;

    auto conn_info = ozo::connection_info(argv[1]);

    ozo::connection_pool_config conn_pool_config;

    // Maximum limit for number of stored connections
    conn_pool_config.capacity = 3;
    // Maximum limit for number of waiting requests for connection
    conn_pool_config.queue_capacity = 10;
    // Maximum time duration to store unused open connection
    conn_pool_config.idle_timeout = 1s;
    // Maximum time duration to keep connection open
    conn_pool_config.lifespan = 24h;

    // Creating connection pool from connection_info as the underlying ConnectionSource
    ozo::connection_pool conn_pool(conn_info, conn_pool_config);

    // Use timer to add a pause between requests
    asio::steady_timer timer(io);

    asio::spawn(io, [&] (asio::yield_context yield) {
        const auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < 10s) {
            const auto stats = conn_pool.stats();
            std::cout << "Connection pool stats: size=" << stats.size
                << " available=" << stats.available << " used=" << stats.used << std::endl;

            ozo::rows_of<int> result;
            ozo::error_code ec;
            using namespace ozo::literals;
            using namespace std::chrono_literals;
            namespace failover = ozo::failover;
            using retry_options = failover::retry_options;
            // Here we will retry operation no more than 3 times on connection errors
            // Each try will have its own time constraint
            //   1st try will be limited by 1/3 sec.
            //   2nd try will be limited by  (1 - t(1st try)) / 2 sec, which is not less than 1/3 sec.
            //   3rd try will be limited by  1 - (t(1st try) + t(2nd try)), which is not less than 1/3 sec too.
            auto retry = 3*failover::retry(ozo::errc::connection_error)
            // We want to print out information about retries
                            .set(retry_options::on_retry = retry_error);
            // Here a request call with retry fallback strategy
            auto conn = ozo::request[retry](conn_pool[io], "SELECT 1"_SQL, 1s, ozo::into(result), yield[ec]);

            // When request is completed we check is there an error.
            if (ec) {
                std::cout << "Request failed; ";
                print_error(ec, conn);
                continue;
            }

            // Just print request result
            std::cout << "Selected:" << std::endl;
            for (auto value : result) {
                std::cout << std::get<0>(value) << std::endl;
            }

            timer.expires_after(300ms);
            timer.async_wait(yield[ec]);
        }
    });

    io.run();

    return 0;
}
