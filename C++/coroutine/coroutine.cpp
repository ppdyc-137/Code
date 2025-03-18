#include <coroutine>
#include <exception>
#include <iostream>
#include <memory>

struct RepeatAwaiter {
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> coroutine) const noexcept {
        if (coroutine.done())
            return std::noop_coroutine();
        else
            return coroutine;
    }

    void await_resume() const noexcept {}
};

struct PreviousAwaiter {
    PreviousAwaiter(std::coroutine_handle<> previousHandle) : previousHandle_(previousHandle) {}

    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<>) const noexcept {
        if (previousHandle_) {
            return previousHandle_;
        } else
            return std::noop_coroutine();
    }

    void await_resume() const noexcept {}

    std::coroutine_handle<> previousHandle_;
};

template <typename T>
struct Promise {
    Promise() {}
    Promise(Promise&&) = delete;
    ~Promise() {}

    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return PreviousAwaiter{previousHandle_}; }
    void unhandled_exception() { exception_ = std::current_exception(); }

    auto yield_value(T ret) {
        std::construct_at(&ret_, std::move(ret));
        return PreviousAwaiter{previousHandle_};
    }

    void return_value(T ret) { std::construct_at(&ret_, std::move(ret)); }

    T result() {
        if (exception_) {
            std::rethrow_exception(exception_);
        }
        auto val = std::move(ret_);
        std::destroy_at(&ret_);
        return val;
    }

    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

    std::coroutine_handle<> previousHandle_{};
    std::exception_ptr exception_{};
    union {
        T ret_{};
    };
};

template <>
struct Promise<void> {
    auto initial_suspend() { return std::suspend_always{}; }
    auto final_suspend() noexcept { return PreviousAwaiter{previousHandle_}; }
    void unhandled_exception() { exception_ = std::current_exception(); }

    void return_void() {}

    void result() {
        if (exception_) {
            std::rethrow_exception(exception_);
        }
    }

    std::coroutine_handle<Promise> get_return_object() { return std::coroutine_handle<Promise>::from_promise(*this); }

    std::coroutine_handle<> previousHandle_{};
    std::exception_ptr exception_{};
};

template <typename T>
struct Task {
    using promise_type = Promise<T>;

    struct Awaiter {
        bool await_ready() const noexcept { return false; }

        auto await_suspend(std::coroutine_handle<> handle) const noexcept {
            handle_.promise().previousHandle_ = handle;
            return handle_;
        }

        T await_resume() const noexcept { return handle_.promise().result(); }

        std::coroutine_handle<promise_type> handle_;
    };

    Task(std::coroutine_handle<promise_type> coroutine) : handle_(coroutine) {}
    auto operator co_await() { return Awaiter(handle_); }
    std::coroutine_handle<promise_type> handle_;
};

Task<void> bar() {
    co_return;
}

Task<std::string> foo() {
    co_yield "hello";
    co_return "world";
}

Task<std::string> hello() {
    co_await bar();

    auto task = foo();

    auto ret = co_await task;
    co_yield ret;

    ret = co_await task;
    co_yield ret;

    co_return "end";
}

int main() {
    Task t = hello();
    while (!t.handle_.done()) {
        t.handle_.resume();
        std::cout << "hello return " << t.handle_.promise().result() << '\n';
    }
    return 0;
}
