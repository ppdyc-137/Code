#include <chrono>
#include <functional>
#include <iostream>
#include <thread>
#include <type_traits>
#include <utility>
#include <future>

void callee(int& x) {
    x = 0;
    std::cout << "lvalue reference" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    x = 0;
}

void callee(int&& x) {
    std::cout << "rvalue reference" << std::endl;
}

template<typename Fn, typename ...Args>
auto spawn_task(Fn&& f, Args&& ...args) {
    using return_type = std::invoke_result_t<Fn, Args...>;
    auto task = std::packaged_task<return_type()>([=]() mutable {
            return std::invoke(std::forward<Fn>(f), std::forward<Args>(args)...);
            });
    auto future = task.get_future();
    std::thread(std::move(task)).detach();
    return future;
}

auto foo() {
    int a = 10;
    auto res = spawn_task<void(int&)>(callee, a);
    // auto res = std::async<void(int&)>(callee, std::ref(a));
    return res;
}

int main() {
    auto res = foo();
    res.get();
}

