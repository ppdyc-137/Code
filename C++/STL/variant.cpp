#include <exception>
#include <algorithm>
#include <iostream>
#include <string>
#include <type_traits>
#include <functional>

template<size_t I>
struct InPlaceIndex {
    explicit InPlaceIndex() = default;
};

template<size_t I>
constexpr InPlaceIndex<I> inPlaceIndex;

struct BadVariantAccess : std::exception {
    BadVariantAccess() = default;
    virtual ~BadVariantAccess() = default;
    const char *what() const noexcept override {
        return "BadVariantAccess";
    }
};

template <typename, typename>
struct VariantIndex;

template <typename, size_t>
struct VariantAlternative;

template<typename ...Ts>
class Variant {
private:
    size_t m_index;
    alignas(std::max({alignof(Ts)...})) char m_union[std::max({sizeof(Ts)...})];

    using DestroyFunc = void (*)(char *) noexcept;
    inline static DestroyFunc desctructor_table[sizeof...(Ts)] = {
        [](char *m_union) noexcept {
            Ts* p = reinterpret_cast<Ts *>(m_union);
            p->~Ts();
        }...
    };

    template<typename Lambda>
    using VisitFunc = void (*)(char*, Lambda &&lambda);

    template<typename Lambda>
    static VisitFunc<Lambda>* get_visitor() {
        static VisitFunc<Lambda> visitor_table[sizeof...(Ts)] = {
            [](char *m_union, Lambda &&lambda) {
                std::invoke(std::forward<Lambda>(lambda), *reinterpret_cast<Ts *>(m_union));
            }...
        };
        return visitor_table;
    }

    template<typename Lambda>
    using ConstVisitFunc = void (*)(const char*, Lambda &&lambda);

    template<typename Lambda>
    static ConstVisitFunc<Lambda>* const_get_visitor() {
        static ConstVisitFunc<Lambda> const_visitor_table[sizeof...(Ts)] = {
            [](const char *m_union, Lambda &&lambda) {
                std::invoke(std::forward<Lambda>(lambda), *reinterpret_cast<const Ts *>(m_union));
            }...
        };
        return const_visitor_table;
    }

public:
    template<typename T, std::enable_if_t<std::disjunction_v<std::is_same<T, Ts>...>, int> = 0>
    Variant(T value) : m_index(VariantIndex<Variant, T>::value) {
        new (m_union) T(value);
    }

    template<size_t I, typename ...Args>
    explicit Variant(InPlaceIndex<I>, Args&& ...args) : m_index(I) {
        using T = typename VariantAlternative<Variant, I>::type;
        new (m_union) T(std::forward<Args>(args)...);
    }

    ~Variant() noexcept {
        desctructor_table[m_index](m_union);
    }

    Variant(const Variant&) = delete;
    Variant& operator=(const Variant &) = delete;

    size_t index() const {
        return m_index;
    }

    template<typename T>
    constexpr bool holds_alternative() const {
        return VariantIndex<Variant, T>::value == m_index;
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
    typename VariantAlternative<Variant, I>::type& get() {
        if ( m_index != I)
            throw BadVariantAccess();
        static_assert(I < sizeof...(Ts), "I out of range");

        using T = typename VariantAlternative<Variant, I>::type;
        return *reinterpret_cast<T*>(m_union);
    }
    template<size_t I>
    const typename VariantAlternative<Variant, I>::type& get() const {
        if ( m_index != I)
            throw BadVariantAccess();
        static_assert(I < sizeof...(Ts), "I out of range");

        using T = typename VariantAlternative<Variant, I>::type;
        return *reinterpret_cast<const T*>(m_union);
    }

    template<typename Lambda>
    void visit(Lambda &&lambda) {
        get_visitor<Lambda>()[m_index](m_union, std::forward<Lambda>(lambda));
    }

    template<typename Lambda>
    void visit(Lambda &&lambda) const {
        const_get_visitor<Lambda>()[m_index](m_union, std::forward<Lambda>(lambda));
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
    Variant<std::string, int, double> v { InPlaceIndex<2>() , 42.1 };
    v.visit([](auto& value) { std::cout << value << "\n"; });
}
