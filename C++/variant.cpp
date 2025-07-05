#include <algorithm>
#include <exception>
#include <type_traits>
#include <string>
#include <iostream>

struct BadVariantAccess : std::exception {
    BadVariantAccess() = default;
    virtual ~BadVariantAccess() = default;

    const char* what() const noexcept override {
        return "BadVariantAccess";
    }
};

template <typename, typename>
struct VariantIndex;

template <typename, size_t>
struct VariantAlternative;

template <typename ...Ts>
class Variant
{
private:
    size_t index_;
    alignas(std::max({alignof(Ts)...})) char union_[std::max({sizeof(Ts)...})];

public:
    template<typename T, std::enable_if_t<std::disjunction_v<std::is_same<T, Ts>...>, int> = 0>
    Variant(T value) : index_(VariantIndex<Variant, T>::value) {
        T *p = reinterpret_cast<T*>(union_);
        new (p) T(value);
    }

    ~Variant() {
    }

    constexpr size_t index() const noexcept {
        return index_;
    }

    template<typename T>
    T& get() {
        return get<VariantIndex<Variant, T>::value>();
    }

    template<typename T>
    const T& get() const {
        return get<VariantIndex<Variant, T>::value>();
    }

    template<size_t I>
    auto& get() {
        if(I != index_)
            throw BadVariantAccess();
        static_assert(I < sizeof...(Ts), "I out of range");
        using T = typename VariantAlternative<Variant, I>::type;
        return *reinterpret_cast<T*>(union_);
    }

    template<size_t I>
    const auto& get() const {
        if(I != index_)
            throw BadVariantAccess();
        static_assert(I < sizeof...(Ts), "I out of range");
        using T = typename VariantAlternative<Variant, I>::type;
        return *reinterpret_cast<const T*>(union_);
    }

    template<typename T>
    constexpr bool holds_alternative() const {
        return VariantIndex<Variant, T>::value == index_;
    }
};

template <typename T, typename ...Ts>
struct VariantIndex<Variant<T, Ts...>, T> {
    static constexpr size_t value = 0;
};

template <typename T0, typename T, typename ...Ts>
struct VariantIndex<Variant<T0, Ts...>, T> {
    static constexpr size_t value = VariantIndex<Variant<Ts...>, T>::value + 1;
};

template <typename T, typename ...Ts>
struct VariantAlternative<Variant<T, Ts...>, 0> {
    using type = T;
};

template <typename T, typename ...Ts, size_t I>
struct VariantAlternative<Variant<T, Ts...>, I> {
    using type = typename VariantAlternative<Variant<Ts...>, I - 1>::type;
};

int main() {
    Variant<std::string, int, double> v(std::string("hello"));
    std::cout << v.get<std::string>() << std::endl;
}
