#pragma once

#include "test_error.h"

#include <ozo/detail/bind.h>
#include <ozo/asio.h>

#include <boost/asio/post.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/bind_executor.hpp>

#include <gmock/gmock.h>

namespace ozo {
namespace tests {

struct executor_mock {
    MOCK_CONST_METHOD1(dispatch, void (std::function<void ()>));
    MOCK_CONST_METHOD1(post, void (std::function<void ()>));
    MOCK_CONST_METHOD1(defer, void (std::function<void ()>));
};

template <typename Handler>
struct shared_wrapper {
    std::shared_ptr<Handler> ptr;

    template <typename ... Args>
    void operator ()(Args&& ... args) {
        return (*ptr)(std::forward<Args>(args) ...);
    }
};

template <typename Function>
auto wrap_shared(Function&& f) {
    return shared_wrapper<std::decay_t<Function>> {std::make_shared<std::decay_t<Function>>(std::forward<Function>(f))};
}

struct strand_service_mock {
    MOCK_METHOD0(get_executor, executor_mock& ());
};

struct steady_timer_mock  {
    MOCK_METHOD1(expires_after, std::size_t (const asio::steady_timer::duration&));
    MOCK_METHOD1(expires_at, std::size_t (const asio::steady_timer::time_point&));
    MOCK_METHOD1(async_wait, void (std::function<void(error_code)>));
    MOCK_METHOD0(cancel, std::size_t ());
};

struct steady_timer_service_mock {
    MOCK_METHOD0(timer, steady_timer_mock& ());
    MOCK_METHOD1(timer, steady_timer_mock& (asio::steady_timer::duration));
    MOCK_METHOD1(timer, steady_timer_mock& (asio::steady_timer::time_point));
};

struct stream_descriptor_mock {
    MOCK_METHOD1(async_write_some, void(std::function<void(error_code)>));
    MOCK_METHOD1(async_read_some, void(std::function<void(error_code)>));
    MOCK_METHOD1(cancel, void(error_code&));
    MOCK_METHOD1(close, void(error_code&));
    MOCK_METHOD0(release, int());
    MOCK_METHOD1(assign, void(int));
};

struct stream_descriptor_service_mock {
    MOCK_METHOD0(create, stream_descriptor_mock& ());
    MOCK_METHOD1(create, stream_descriptor_mock& (int));
};

template <typename Executor>
struct steady_timer {
    steady_timer_mock* impl = nullptr;
    Executor executor_;

    std::size_t expires_after(const asio::steady_timer::duration& expiry_time) {
        return impl->expires_after(expiry_time);
    }

    std::size_t expires_at(const asio::steady_timer::time_point& at) {
        return impl->expires_at(at);
    }

    template <typename Handler>
    void async_wait(Handler&& handler) {
        return impl->async_wait([h = std::forward<Handler>(handler)] (auto e) {
            asio::post(ozo::detail::bind(std::move(h), std::move(e)));
        });
    }

    std::size_t cancel() {
        return impl->cancel();
    }

    using executor_type = Executor;

    executor_type get_executor() const { return executor_; }
};

struct execution_context : asio::execution_context {
    struct executor_type {
        executor_mock* impl_ = nullptr;
        execution_context* context_ = nullptr;

        executor_type(executor_mock& impl, execution_context& context)
        : impl_(&impl), context_(&context){}

        explicit executor_type(execution_context& context)
        : context_(&context){}

        explicit executor_type(executor_mock& impl)
        : impl_(&impl) {}

        executor_type() = default;

        execution_context& context() const noexcept {
            return *context_;
        }

        void on_work_started() const {}

        void on_work_finished() const {}

        template <typename Function, typename Allocator>
        void dispatch(Function&& f, Allocator&&) const {
            assert_has_impl();
            return impl_->dispatch(wrap_shared(std::forward<Function>(f)));
        }

        template <typename Function, typename Allocator>
        void post(Function&& f, Allocator&&) const {
            assert_has_impl();
            return impl_->post(wrap_shared(std::forward<Function>(f)));
        }

        template <typename Function, typename Allocator>
        void defer(Function&& f, Allocator&&) const {
            assert_has_impl();
            return impl_->defer(wrap_shared(std::forward<Function>(f)));
        }

        void assert_has_impl() const {
            if (!impl_) {
                throw std::logic_error("ozo::testing::execution_context::executor_type::assert_impl() no executor mock");
            }
        }

        friend bool operator ==(const executor_type& lhs, const executor_type& rhs) {
            return lhs.context_ == rhs.context_ && lhs.impl_ == rhs.impl_;
        }

        friend bool operator !=(const executor_type& lhs, const executor_type& rhs) {
            return !(lhs == rhs);
        }
    };

    testing::StrictMock<executor_mock> executor_;
    testing::StrictMock<strand_service_mock> strand_service_;
    testing::StrictMock<steady_timer_service_mock> timer_service_;
    testing::StrictMock<stream_descriptor_service_mock> stream_service_;

    executor_type get_executor() { return {executor_, *this}; }
};

using io_context = execution_context;
using executor = execution_context::executor_type;
using strand = executor;

struct stream_descriptor {
    io_context* io_ = nullptr;
    stream_descriptor_mock* mock_ = nullptr;

    using native_handle_type = int;

    stream_descriptor(io_context& io, stream_descriptor_mock& mock)
    : io_(&io), mock_(&mock) {}

    stream_descriptor(io_context& io, int fd)
    : io_(&io), mock_(&io.stream_service_.create(fd)) {}

    stream_descriptor(io_context& io)
    : io_(&io), mock_(&io.stream_service_.create()) {}

    template <typename ConstBufferSequence, typename WriteHandler>
    void async_write_some(ConstBufferSequence const &, WriteHandler&& h) {
        mock_->async_write_some([h = std::forward<WriteHandler>(h)] (auto e) {
            asio::post(ozo::detail::bind(std::move(h), std::move(e)));
        });
    }

    template <typename BufferSequence, typename ReadHandler>
    void async_read_some(BufferSequence&&, ReadHandler&& h) {
        mock_->async_read_some([h = std::forward<ReadHandler>(h)] (auto e) {
            asio::post(ozo::detail::bind(std::move(h), std::move(e)));
        });
    }

    void cancel(error_code& ec) { mock_->cancel(ec);}

    void close(error_code& ec) { mock_->close(ec);}

    void release() { mock_->release();}

    void assign(int fd) { mock_->assign(fd);}

    using executor_type = boost::asio::executor;

    executor_type get_executor() const {
        return io_->get_executor();
    }
};

} // namespace tests

namespace detail {

template <>
struct strand_executor<ozo::tests::executor> {
    using type = ozo::tests::executor;

    static auto get(const tests::executor& ex) {
        return type{ex.context().strand_service_.get_executor(), ex.context()};
    }
};

template <>
struct operation_timer<ozo::tests::executor> {
    using type = ozo::tests::steady_timer<ozo::tests::executor>;

    template <typename TimeConstraint>
    static type get(const ozo::tests::executor& ex, TimeConstraint t) {
        return type{std::addressof(ex.context_->timer_service_.timer(t)), ex};
    }

    template <typename TimeConstraint>
    static type get(const ozo::tests::executor& ex) {
        return type{std::addressof(ex.context_->timer_service_.timer()), ex};
    }
};

} // namespace detail

namespace tests {

template <typename ... Args>
struct callback_mock {
    virtual void call(ozo::error_code, Args...) const = 0;
    virtual ~callback_mock() = default;
};

template <typename ...Args>
struct callback_gmock;

template <typename Arg1, typename Arg2>
struct callback_gmock<Arg1, Arg2> {
    using executor_type = boost::asio::executor;

    MOCK_CONST_METHOD3_T(call, void(ozo::error_code, Arg1, Arg2));
    MOCK_CONST_METHOD0_T(get_executor, executor_type ());
};

template <typename Arg>
struct callback_gmock<Arg> {
    using executor_type = boost::asio::executor;

    MOCK_CONST_METHOD2_T(call, void(ozo::error_code, Arg));
    MOCK_CONST_METHOD0_T(get_executor, executor_type ());
};

template <>
struct callback_gmock<> {
    using executor_type = boost::asio::executor;

    MOCK_CONST_METHOD1_T(call, void(ozo::error_code));
    MOCK_CONST_METHOD0_T(get_executor, executor_type ());
};

template <typename M>
struct callback_handler {
    using executor_type = typename M::executor_type;

    M* mock_ = nullptr;

    callback_handler(M& mock) : mock_(std::addressof(mock)) {}

    template <typename ...Args>
    void operator ()(ozo::error_code ec, Args&&... args) const {
        mock_->call(ec, std::forward<Args>(args)...);
    }

    auto get_executor() const noexcept {
        return mock_->get_executor();
    }
};

template <typename T>
inline callback_handler<typename T::type> wrap(T& mock) {
    return {object(mock)};
}

template <typename ...Ts>
inline callback_handler<callback_gmock<Ts...>> wrap(callback_gmock<Ts...>& mock) {
    return {mock};
}

template <typename ...Ts>
inline callback_handler<callback_gmock<Ts...>> wrap(testing::StrictMock<callback_gmock<Ts...>>& mock) {
    return {mock};
}

template <typename T, typename Executor>
inline auto wrap(T& mock, const Executor& e) {
    return asio::bind_executor(e, callback_handler<typename T::type>{object(mock)});
}

template <typename Executor, typename ...Ts>
inline auto wrap(callback_gmock<Ts...>& mock, const Executor& e) {
    return asio::bind_executor(e, callback_handler<callback_gmock<Ts...>>{mock});
}

template <typename Executor, typename ...Ts>
inline auto wrap(testing::StrictMock<callback_gmock<Ts...>>& mock, const Executor& e) {
    return asio::bind_executor(e, callback_handler<callback_gmock<Ts...>>{mock});
}

} // namespace tests
} // namespace ozo
