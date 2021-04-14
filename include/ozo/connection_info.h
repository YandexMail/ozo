#pragma once

#include <ozo/connector.h>
#include <ozo/connection.h>
#include <ozo/impl/async_connect.h>
#include <ozo/ext/std/shared_ptr.h>

#include <chrono>

namespace ozo {

/**
 * @brief Connection source to a single host
 *
 * This connection source establishes a connection to a single host using (or via) the specified
 * [connection string](https://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-CONNSTRING).
 *
 * @warning Multi-host connection is not supported.
 *
 * @tparam OidMap --- oid map type with custom types that should be used within a connection.
 * @tparam Statistics --- statistics type which defines statistics is collected for this connection.
 * @ingroup group-connection-types
 * @models{ConnectionSource}
 */
template <
    typename OidMap = empty_oid_map,
    typename Statistics = no_statistics>
class connection_info {
    std::string conn_str;
    Statistics statistics;

public:
    using connection_type = std::shared_ptr<ozo::connection<std::decay_t<OidMap>, Statistics>>; //!< Type of connection which is produced by the source.

    /**
     * @brief Construct a new connection information object
     *
     * @param conn_str --- connection string which is being used to create connection to a database.
     * For details of how to make string see [official libpq documentation](https://www.postgresql.org/docs/9.4/static/libpq-connect.html#LIBPQ-CONNSTRING)
     * @param OidMap --- #OidMap for custom types support.
     * @param statistics --- statistics are being used for connections.
     */
    connection_info(std::string conn_str, const OidMap& = OidMap{}, Statistics statistics = Statistics{})
            : conn_str(std::move(conn_str)), statistics(std::move(statistics)) {
    }

    /**
     * @brief Provides connection is binded to the given `io_context`
     *
     * In case of success --- the handler will be invoked as operation succeeded.
     * In case of connection fail --- the handler will be invoked as operation failed.
     * This operation has a time constrain and would be interrupted if the time
     * constrain expired by cancelling IO on a `Connection`'s socket.
     *
     * @param io --- `io_context` for the connection IO.
     * @param t --- #TimeConstraint for the operation.
     * @param handler --- #Handler.
     */
    template <typename TimeConstraint, typename Handler>
    void operator ()(io_context& io, TimeConstraint t, Handler&& handler) const {
        static_assert(ozo::TimeConstraint<TimeConstraint>, "should model TimeConstraint concept");
        auto allocator = asio::get_associated_allocator(handler);
        impl::async_connect(conn_str, t, std::allocate_shared<ozo::connection<OidMap, Statistics>>(allocator, io, statistics),
            std::forward<Handler>(handler));
    }

    auto operator [](io_context& io) const & {
        return connection_provider(*this, io);
    }

    auto operator [](io_context& io) && {
        return connection_provider(std::move(*this), io);
    }
};

template<typename OidMap, typename Statistics>
connection_info(std::string, const OidMap&, Statistics statistics) -> connection_info<OidMap, Statistics>;

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(const connection_info<Ts...>& source, io_context& io, time_traits::duration timeout) {
    return bind_get_connection_timeout(source[io], timeout);
}

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(connection_info<Ts...>& source, io_context& io, time_traits::duration timeout) {
    return bind_get_connection_timeout(source[io], timeout);
}

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(connection_info<Ts...>&& source, io_context& io, time_traits::duration timeout) {
    return bind_get_connection_timeout(source[io], timeout);
}

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(const connection_info<Ts...>& source, io_context& io) { return source[io];}

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(connection_info<Ts...>& source, io_context& io) { return source[io];}

//[[DEPRECATED]] for backward compatibility only
template <typename ...Ts>
[[deprecated]] auto make_connector(connection_info<Ts...>&& source, io_context& io) { return source[io];}

/**
 * @brief Constructs `ozo::connection_info` `ConnectionSource`.
 * @ingroup group-connection-functions
 * @relates ozo::connection_info
 *
 * @param conn_str --- standard libpq connection string.
 * @param OidMap --- oid map for user defined types.
 * @param statistics --- statistics to collect for a connection.
 * @return `ozo::connection_info` specialization.
 */
template <typename OidMap = empty_oid_map, typename Statistics = no_statistics>
inline auto make_connection_info(std::string conn_str, const OidMap& oid_map = OidMap{},
        Statistics statistics = Statistics{}) {
    return connection_info{std::move(conn_str), oid_map, statistics};
}

static_assert(ConnectionProvider<decltype(std::declval<connection_info<>>()[std::declval<io_context&>()])>, "is not a ConnectionProvider");

} // namespace ozo
