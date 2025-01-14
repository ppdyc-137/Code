#include <cassert>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <format>
#include <type_traits>

template<typename T, std::size_t N>
struct Array {
    T _M_elems[N];

    static constexpr std::size_t size() { return N; }
    static constexpr std::size_t max_size() { return N; }
    constexpr bool empty() const { return N == 0; }

    T& at(std::size_t pos)
    {
        if (pos >= N)
            throw std::out_of_range(std::format("{} out of range {}", pos, N));
        return _M_elems[pos];
    }

    const T& at(std::size_t pos) const {
        if (pos >= N)
            throw std::out_of_range(std::format("{} out of range {}", pos, N));
        return _M_elems[pos];
    }

    T& operator[](std::size_t pos) noexcept{ return _M_elems[pos]; }
    const T& operator[](std::size_t pos) const noexcept { return _M_elems[pos]; }

    T& front() noexcept { return _M_elems[0]; }
    const T& front() const noexcept { return _M_elems[0]; }

    T& back() noexcept { return _M_elems[N - 1]; }
    const T& back() const noexcept { return _M_elems[N - 1]; }

    T* data() noexcept { return _M_elems; }
    const T* data() const noexcept { return _M_elems; }

    T* begin() noexcept { return _M_elems; }
    const T* begin() const noexcept { return _M_elems; }
    const T* cbegin() const noexcept { return _M_elems; }

    T* end() noexcept { return _M_elems + N; }
    const T* end() const noexcept { return _M_elems + N; }
    const T* cend() const noexcept { return _M_elems + N; }

    void fill(const T& val) noexcept(noexcept(std::is_nothrow_copy_assignable_v<T>))
    {
        for (std::size_t i = 0; i < N; i++) {
            _M_elems[i] = val;
        }
    }

    void swap(Array& that) noexcept(noexcept(std::is_nothrow_swappable_v<T>))
    {
        for (std::size_t i = 0; i < N; i++) {
            std::swap(_M_elems[i], that._M_elems[i]);
        }
    }
};

template <typename T>
struct Array<T, 0> {
    static constexpr std::size_t size() { return 0; }
    static constexpr std::size_t max_size() { return 0; }
    static constexpr bool empty() { return true; }

    T& at(std::size_t pos) { throw std::out_of_range(std::format("{} out of range {}", pos, 0)); }

    const T& at(std::size_t pos) const { throw std::out_of_range(std::format("{} out of range {}", pos, 0)); }

    T& operator[](std::size_t pos) noexcept { assert(0); }
    const T& operator[](std::size_t pos) const noexcept { assert(0); }

    T& front() noexcept { return nullptr; }
    const T& front() const noexcept { return nullptr; }

    T& back() noexcept{ return nullptr; }
    const T& back() const noexcept{ return nullptr; }

    T* data() noexcept { return nullptr; }
    const T* data() const noexcept { return nullptr; }

    T* begin() noexcept { return nullptr; }
    const T* begin() const noexcept { return nullptr; }
    const T* cbegin() const noexcept { return nullptr; }

    T* end() noexcept { return nullptr; }
    const T* end() const noexcept { return nullptr; }
    const T* cend() const noexcept { return nullptr; }

    void fill(const T& val) noexcept {}

    void swap(Array& that) noexcept {}
};

template<typename T, typename ...Ts>
Array(T, Ts...) -> Array<T, 1 + sizeof...(Ts)>;

template <typename T, std::size_t N, std::enable_if_t<std::is_copy_constructible_v<T>, int> = 0>
Array<std::remove_cv_t<T>, N> toArray(T (&val)[N]) {
    Array<std::remove_cv_t<T>, N> arr;
    for (std::size_t i = 0; i < N; ++i) {
        arr._M_elems[i] = val[i];
    }
    return arr;
}

template <typename T, std::size_t N, std::enable_if_t<std::is_move_constructible_v<T>, int> = 0>
Array<std::remove_cv_t<T>, N> toArray(T (&&val)[N]) {
    Array<std::remove_cv_t<T>, N> arr;
    for (std::size_t i = 0; i < N; ++i) {
        arr._M_elems[i] = std::move(val[i]);
    }
    return arr;
}

int main() {
    Array b{ 1, 2, 3};
    int a[] = { 1, 2, 3};
    auto c = toArray(a);

    std::cout << '\n';
}
