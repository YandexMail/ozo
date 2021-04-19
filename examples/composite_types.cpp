#include <ozo/connection_info.h>
#include <ozo/request.h>
#include <ozo/execute.h>
#include <ozo/transaction.h>
#include <ozo/shortcuts.h>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/hana/adapt_struct.hpp>
#include <boost/hana/members.hpp>
#include <boost/hana/size.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>

#include <iostream>

namespace asio = boost::asio;
namespace hana = boost::hana;

struct attach {
    std::string filename;
    std::string type;
    std::int64_t size;
};

BOOST_HANA_ADAPT_STRUCT(attach, filename, type, size);
BOOST_FUSION_ADAPT_STRUCT(attach, filename, type, size)

OZO_PG_DEFINE_CUSTOM_TYPE(attach, "ozo_test.attach")
OZO_PG_DEFINE_CUSTOM_TYPE(std::vector<attach>, "ozo_test.attach[]")

const auto throw_if_error = [](ozo::error_code ec, const auto& conn) {
    if (ec) {
        std::ostringstream s;
        if (!ozo::is_null_recursive(conn)) {
            s << "libpq error message: \"" << ozo::error_message(conn)
                << "\", error context: \"" << ozo::get_error_context(conn) << "\"";
        }
        throw ozo::system_error(ec, s.str());
    }
};

std::ostream& operator <<(std::ostream& stream, const attach& value) {
    stream << "attach {";
    hana::for_each(hana::members(value), [&] (const auto& v) { stream << v << ", "; });
    return stream << "}";
}

template <class T>
std::ostream& operator <<(std::ostream& stream, const std::vector<T>& value) {
    stream << "{";
    boost::for_each(value, [&] (const auto& v) { stream << v << ", "; });
    return stream << "}";
}

template <class ... T>
std::ostream& operator <<(std::ostream& stream, const hana::tuple<std::reference_wrapper<T> ...>& value) {
    stream << "{";
    hana::for_each(value, [&] (const auto& v) { stream << v.get() << ", "; });
    return stream << "}";
}

template <class ... T>
std::ostream& operator <<(std::ostream& stream, const hana::tuple<T ...>& value) {
    stream << "{";
    hana::for_each(value, [&] (const auto& v) { stream << v << ", "; });
    return stream << "}";
}

template <class ... T>
std::ostream& operator <<(std::ostream& stream, const std::tuple<T ...>& value) {
    stream << "{";
    hana::for_each(value, [&] (const auto& v) { stream << v << ", "; });
    return stream << "}";
}

template <typename T>
void create_database(T initiator, asio::yield_context yield) {
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    constexpr auto init_queries = hana::make_tuple(
        "DROP SCHEMA IF EXISTS ozo_test CASCADE;"_SQL,
        "CREATE SCHEMA ozo_test;"_SQL,
        "CREATE TYPE ozo_test.attach AS (filename text, type text, size bigint);"_SQL,
        "CREATE TABLE ozo_test.messages (uid bigint, mid bigint, attaches ozo_test.attach[]);"_SQL
    );

    hana::for_each(init_queries, [&] (const auto& query_builder) {
        const auto query = query_builder.build();

        std::cout << "Perform request with query: " << ozo::to_const_char(ozo::get_text(query)) << std::endl;

        ozo::error_code ec;
        const auto connection = ozo::execute(initiator, query, 3s, yield[ec]);
        throw_if_error(ec, connection);
    });
}

template <typename T>
T fill_database(T&& transaction, asio::yield_context yield) {
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    std::int64_t uid = 0;
    std::int64_t mid = 0;
    std::vector<attach> attaches;
    const auto query_builder = "INSERT INTO ozo_test.messages (uid, mid, attaches) VALUES ("_SQL
            + std::cref(uid) + ", "_SQL + std::cref(mid) + ", "_SQL + std::cref(attaches) + ")"_SQL;

    const auto values = {
        std::make_tuple(std::int64_t(1), std::int64_t(1), std::vector<attach>({})),
        std::make_tuple(std::int64_t(1), std::int64_t(2), std::vector<attach>({
            attach {"foo.jpeg", "image/jpeg", 13124}
        })),
        std::make_tuple(std::int64_t(1), std::int64_t(3), std::vector<attach>({
            attach {"report.txt", "text/plain", 5344},
            attach {"doc.txt", "text/plain", 3434}
        })),
        std::make_tuple(std::int64_t(2), std::int64_t(1), std::vector<attach>({})),
        std::make_tuple(std::int64_t(2), std::int64_t(2), std::vector<attach>({})),
    };

    boost::for_each(values, [&] (const auto& value) {
        ozo::error_code ec;
        std::tie(uid, mid, attaches) = value;
        const auto query = query_builder.build();
        std::cout << "Perform request with query: " << ozo::to_const_char(ozo::get_text(query))
                  << ", params: " << ozo::get_params(query)
                  << std::endl;
        transaction = ozo::execute(std::move(transaction), query, 3s, yield[ec]);
        throw_if_error(ec, transaction);
    });

    return std::move(transaction);
}

template <typename T>
T query_database(T&& transaction, asio::yield_context yield) {
    using namespace ozo::literals;
    using namespace std::chrono_literals;

    const auto query = (
        "SELECT mid, attaches "_SQL +
          "FROM ozo_test.messages "_SQL +
         "WHERE uid = "_SQL + std::int64_t(1) +
          " AND mid = ANY("_SQL + std::vector<std::int64_t>({2, 3}) + ")"_SQL
    ).build();

    ozo::rows_of<std::int64_t, std::vector<attach>> result;
    ozo::error_code ec;
    std::cout << "Perform request with query: " << ozo::to_const_char(ozo::get_text(query))
              << ", params: " << ozo::get_params(query)
              << std::endl;
    ozo::request(transaction, query, 3s, std::back_inserter(result), yield[ec]);
    throw_if_error(ec, transaction);

    std::cout << "Selected attaches:" << std::endl;
    boost::for_each(result, [&] (const auto& v) {
        std::cout << std::get<0>(v) << ", " << std::get<1>(v) << std::endl;
    });

    return std::move(transaction);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <conninfo>\n";
        return 1;
    }

    asio::io_context io;
    asio::spawn(io, [&] (asio::yield_context yield) {
        using namespace ozo::literals;
        using namespace std::chrono_literals;

        try {
            create_database(ozo::connection_info(argv[1])[io], yield);

            const auto oid_map = ozo::register_types<attach, std::vector<attach>>();
            ozo::connection_info connection_info(argv[1], oid_map);
            ozo::error_code ec;
            auto transaction = ozo::begin(connection_info[io], 3s, yield[ec]);
            throw_if_error(ec, transaction);

            transaction = fill_database(std::move(transaction), yield);
            transaction = query_database(std::move(transaction), yield);

            const auto connection = ozo::rollback(std::move(transaction), 3s, yield[ec]);
            throw_if_error(ec, connection);
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    });

    io.run();

    return 0;
}
