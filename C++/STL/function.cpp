#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <type_traits>


template<typename F>
struct Function {
    static_assert(false, "not a valid function signature");
};

template<typename RetType, typename ...Args>
struct Function<RetType(Args...)> {
    struct FunctionBase {
        virtual RetType call(Args ...args) = 0;
        virtual ~FunctionBase() = default;
    };

    template<typename F>
    struct FunctionImpl : FunctionBase {
        FunctionImpl(F func) : f(std::move(func)) {}

        RetType call(Args&& ...args) override {
            return std::invoke(f, std::forward<Args>(args)...);
        }
    private:
        F f;
    };

public:
    Function() = default;

    template<typename Func, std::enable_if_t<std::is_invocable_r_v<RetType, Func, Args...> && std::is_copy_assignable_v<Func> && !std::is_same_v<std::decay_t<Func>, Function>, int> = 0>
    Function(Func func) : m_base(std::make_unique<FunctionImpl<Func>>(std::move(func))) {}

    void operator()(Args&& ...args) {
        if (!m_base)
            throw std::runtime_error("function not initialized");
        return m_base->call(std::forward<Args>(args)...);
    }

private:
    std::unique_ptr<FunctionBase> m_base;
};

void func1(int& i) {
    i = 1;
    std::cout << "1\n";
}

int main() {
    int i = 0;
    Function<void(int&)> f = func1;
    f(i);
    std::cout << i << '\n';
}
