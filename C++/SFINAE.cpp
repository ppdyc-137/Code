#include <type_traits>
template <typename T, typename T0>
struct is_same {
    static constexpr bool value = false;
};

template <typename T>
struct is_same<T, T> {
    static constexpr bool value = true;
};

template <typename T, typename T0>
static constexpr bool is_same_v = is_same<T, T0>::value;

template <bool, typename T>
struct enable_if {
    using type = T;
};

template <typename T>
struct enable_if<false, T> {
};

template <bool flag, typename T>
using enable_if_t = typename enable_if<flag, T>::type;

template <typename T, typename = void>
struct has_foo {
    static constexpr bool value = false;
};

template <typename F>
struct has_foo<F, std::void_t<decltype(std::declval<F>().foo())>> {
    static constexpr bool value = true;
};

struct A {
    void foo() {}
};

struct B {
    void bar() {}
};

int main() {
    if constexpr (has_foo<B>::value) {
    }
}
