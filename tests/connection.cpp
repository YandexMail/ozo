#include <apq/connection.h>

#include <boost/make_shared.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/adapted/struct/define_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>

#include <boost/hana/adapt_adt.hpp>

#include "test_asio.h"

#include <GUnit/GTest.h>

namespace {

namespace hana = ::boost::hana;

enum class native_handle { bad, good };

using libapq::testing::socket_mock;

inline bool connection_status_bad(const native_handle* h) {
    return *h == native_handle::bad;
}

using libapq::empty_oid_map;

template <typename OidMap = empty_oid_map>
struct connection {
    using handle_type = std::unique_ptr<native_handle>;

    handle_type handle_ = std::make_unique<native_handle>();
    socket_mock socket_;
    OidMap oid_map_;
    std::string error_context_;

    friend OidMap& get_connection_oid_map(connection& conn) {
        return conn.oid_map_;
    }
    friend const OidMap& get_connection_oid_map(const connection& conn) {
        return conn.oid_map_;
    }
    friend socket_mock& get_connection_socket(connection& conn) {
        return conn.socket_;
    }
    friend const socket_mock& get_connection_socket(const connection& conn) {
        return conn.socket_;
    }
    friend handle_type& get_connection_handle(connection& conn) {
        return conn.handle_;
    }
    friend const handle_type& get_connection_handle(const connection& conn) {
        return conn.handle_;
    }
    friend std::string& get_connection_error_context(connection& conn) {
        return conn.error_context_;
    }
    friend const std::string& get_connection_error_context(const connection& conn) {
        return conn.error_context_;
    }
};

template <typename ...Ts>
using connection_ptr = std::shared_ptr<connection<Ts...>>;

static_assert(libapq::Connection<connection<>>,
    "connection does not meet Connection requirements");
static_assert(libapq::ConnectionWrapper<connection_ptr<>>,
    "connection_ptr does not meet ConnectionWrapper requirements");
static_assert(libapq::Connectable<connection<>>,
    "connection does not meet Connectable requirements");
static_assert(libapq::Connectable<connection_ptr<>>,
    "connection_ptr does not meet Connectable requirements");

static_assert(!libapq::Connection<int>,
    "int meets Connection requirements unexpectedly");
static_assert(!libapq::ConnectionWrapper<int>,
    "int meets ConnectionWrapper requirements unexpectedly");
static_assert(!libapq::Connectable<int>,
    "int meets Connectable requirements unexpectedly");

GTEST("libapq::connection_good()") {
    SHOULD("for object with bad handle returns false") {
        auto conn = std::make_shared<connection<>>();
        *(conn->handle_) = native_handle::bad;
        EXPECT_FALSE(libapq::connection_good(conn));
    }

    SHOULD("for object with nullptr returns false") {
        connection_ptr<> conn;
        EXPECT_FALSE(libapq::connection_good(conn));
    }

    SHOULD("for object with good handle returns true") {
        auto conn = std::make_shared<connection<>>();
        *(conn->handle_) = native_handle::good;
        EXPECT_TRUE(libapq::connection_good(conn));
    }
}

GTEST("libapq::connection_bad()") {
    SHOULD("for object with bad handle returns true") {
        auto conn = std::make_shared<connection<>>();
        *(conn->handle_) = native_handle::bad;
        EXPECT_TRUE(libapq::connection_bad(conn));
    }

    SHOULD("for object with nullptr returns true") {
        connection_ptr<> conn;
        EXPECT_FALSE(libapq::connection_good(conn));
    }

    SHOULD("for object with good handle returns false") {
        auto conn = std::make_shared<connection<>>();
        *(conn->handle_) = native_handle::good;
        EXPECT_FALSE(libapq::connection_bad(conn));
    }
}

GTEST("libapq::unwrap_connection()") {
    SHOULD("for wrapped connection returns connection reference") {
        auto conn = std::make_shared<connection<>>();

        EXPECT_EQ(
            std::addressof(libapq::unwrap_connection(conn)),
            conn.get()
        );
    }

    SHOULD("for connection returns reference to itself") {
        auto conn = connection<>();

        EXPECT_EQ(
            std::addressof(libapq::unwrap_connection(conn)),
            std::addressof(conn)
        );
    }
}

GTEST("libapq::get_error_context()") {
    SHOULD("for connection returns reference to error_context_") {
        auto conn = std::make_shared<connection<>>();

        EXPECT_EQ(
            std::addressof(libapq::get_error_context(conn)),
            std::addressof(conn->error_context_)
        );
    }
}

GTEST("libapq::set_error_context()") {
    SHOULD("for connection sets error_context_") {
        auto conn = std::make_shared<connection<>>();
        libapq::set_error_context(conn, "brand new super context");

        EXPECT_EQ(conn->error_context_, "brand new super context");
    }
}

GTEST("libapq::reset_error_context()") {
    SHOULD("for connection resets error_context_ into empty string") {
        auto conn = std::make_shared<connection<>>();
        conn->error_context_ = "brand new super context";
        libapq::reset_error_context(conn);
        EXPECT_TRUE(conn->error_context_.empty());
    }
}

GTEST("libapq::get_connection()") {
    using callback_mock = libapq::testing::callback_mock<std::shared_ptr<connection<>>>;
    using libapq::testing::wrap;
    using libapq::error_code;
    using namespace ::testing;

    SHOULD("pass through the connection to handler") {
        auto conn = std::make_shared<connection<>>();
        StrictGMock<callback_mock> cb_mock{};
        EXPECT_INVOKE(cb_mock, context_preserved);
        EXPECT_CALL(cb_mock, (call)(error_code{}, conn));
        libapq::get_connection(conn, wrap(cb_mock));
    }

    SHOULD("resets connection error context") {
        auto conn = std::make_shared<connection<>>();
        conn->error_context_ = "some context here";
        libapq::get_connection(conn, [](error_code, auto conn) {
            EXPECT_TRUE(conn->error_context_.empty());
        });
    }
}

} //namespace