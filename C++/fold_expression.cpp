#include <iostream>
#include <utility>

// void print() {
//     std::cout << "\n";
// }
//
// template<typename T, typename ...Ts>
// void print(T t, Ts ...ts) {
//     std::cout << t << " ";
//     print(ts...);
// }

template<typename ...Ts>
void print(Ts ...ts) {
    (..., (std::cout << ts << ','));
    std::cout << '\n';
}

template <typename ...Ts>
auto sum(Ts ...ts) {
    return (0 + ... + ts);
}

template <typename ...Ts>
constexpr bool all(Ts ...ts) { return (ts && ...); }

template <typename ...Ts>
constexpr bool any(Ts ...ts) { return (ts || ...); }

template <typename Fn, typename ...Ts>
void for_each(Fn f, Ts ...ts) {
    (std::forward<Fn>(f)(ts), ...);
}

int main() {
    // print(1, 2, "hello");
    // sum(1, 2.1);
    // auto res = any();
    for_each([](auto x) { std::cout << x << '\n'; }, 1, 2, 3);
}

