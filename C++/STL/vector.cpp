#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <utility>

template<typename T, typename Alloc = std::allocator<T>>
class Vector {
private:
    T* m_data {};
    size_t m_size {};
    size_t m_cap {};

    [[no_unique_address]] Alloc m_alloc{};
public:
    Vector() = default;

    explicit Vector(const Alloc& alloc) : m_alloc(alloc) {}
    explicit Vector(size_t count, const Alloc& alloc = Alloc()) : m_size(count), m_cap(count), m_alloc(alloc) {
        m_data = m_alloc.allocate(count);
    }

    Vector(size_t count, const T& value, const Alloc& alloc = Alloc()) : m_size(count), m_cap(count), m_alloc(alloc)
    {
        m_data = m_alloc.allocate(count);
        for (size_t i = 0; i < count; i++) {
            std::construct_at(&m_data[i], std::as_const(value));
        }
    }

    template<typename InputIt>
    Vector(InputIt first, InputIt last, const Alloc& alloc = Alloc()) : m_alloc(alloc) {
        auto distance = last - first;
        m_size = distance;
        m_cap = distance;
        m_data = m_alloc.allocate(distance);
        for (size_t i = 0; i < distance; i ++) {
            std::construct_at(&m_data[i], std::as_const(*(first + i)));
        }
    }

    Vector(std::initializer_list<T> init) : Vector(init.begin(), init.end()) {}

    ~Vector() {
        if (m_cap)
            m_alloc.deallocate(m_data, m_cap);
    }

    Vector(const Vector& that) {
        m_size = that.m_size;
        m_cap = that.m_cap;
        m_alloc = that.m_alloc;
        if (m_size == 0) {
            m_data = nullptr;
        } else {
            m_data = m_alloc.allocate(m_size);
            for (size_t i = 0 ; i < m_size ; i ++) {
                std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
            }
        }
    }
    Vector& operator=(const Vector& that) {
        if (m_data)
            m_alloc.deallocate(m_data, m_cap);
        m_size = that.m_size;
        m_cap = that.m_cap;
        m_alloc = that.m_alloc;

        if (m_size == 0) {
            m_data = nullptr;
        } else {
            m_data = m_alloc.allocate(m_size);
            for (size_t i = 0 ; i < m_size ; i ++) {
                std::construct_at(&m_data[i], std::as_const(that.m_data[i]));
            }
        }
        return *this;
    }

    Vector(Vector&& that) noexcept {
        if (this == &that)
            return;

        m_data = std::exchange(that.m_data, nullptr);
        m_size = std::exchange(that.m_size, 0);
        m_cap = std::exchange(that.m_cap, 0);
        m_alloc = that.m_alloc;
    }
    Vector& operator=(Vector&& that) noexcept {
        if (this == &that)
            return *this;

        if (m_data)
            m_alloc.deallocate(m_data, m_cap);
        m_data = std::exchange(that.m_data, nullptr);
        m_size = std::exchange(that.m_size, 0);
        m_cap = std::exchange(that.m_cap, 0);
        m_alloc = that.m_alloc;
        return *this;
    }

    void assign(size_t count, const T& value) {
        reserve(count);
        m_size = count;
        for (size_t i = 0 ; i < count; i++) {
            std::construct_at(&m_data[i], value);
        }
    }
    template<typename InputIt>
    void assign(InputIt first, InputIt last) {
        size_t count = last - first;
        reserve(count);
        m_size = count;
        for (size_t i = 0 ; i < count; i++) {
            std::construct_at(&m_data[i], std::as_const(*(first + i)));
        }
    }
    void assign(std::initializer_list<T> init) { assign(init.begin(), init.end()); }

    void clear() noexcept { resize(0); }

    // void insert(size_t pos, const T& value) {}
    // void insert(size_t pos, T&& value) {}
    // void insert(size_t pos, size_t count, T& value) {}
    // template <typename InputIt>
    // void insert(const T* pos, InputIt first, InputIt last) {}

    T* erase(const T* _pos) {
        auto pos = const_cast<T*>(_pos);

        if (pos == end())
            return end();

        for (auto it = pos; it + 1 != end(); it++) {
            *it = std::move(*(it + 1));
        }
        m_size --;
        return pos;
    }
    T* erase(const T* _first, const T* _last) {
        if (_first == _last)
            return const_cast<T*>(_last);

        auto first = const_cast<T*>(_first);
        auto last = const_cast<T*>(_last);
        auto distance = last - first;
        for (auto it = first; it + distance != end(); it++) {
            *it = std::move(*(it + distance));
        }
        m_size -= distance;
        return first;
    }

    void push_back(const T& value) {
        reserve(m_size + 1);
        std::construct_at(end(), value);
        m_size ++;
    }
    void push_back(T&& value) {
        reserve(m_size + 1);
        std::construct_at(end(), std::move(value));
        m_size ++;
    }

    template<typename ...Args>
    T& emplace_back(Args&& ...args) {
        reserve(m_size + 1);
        std::construct_at(end(), std::forward<Args>(args)...);
        m_size ++;
        return back();
    }

    void resize(size_t count, const T& value = T()) {
        reserve(count);

        if (m_size < count) {
            for (auto i = m_size; i < count; i++) {
                std::construct_at(&m_data[i], value);
            }
        }
        m_size = count;
    }

    bool empty() const noexcept { return m_size == 0; }
    size_t size() const noexcept { return m_size; }
    size_t capacity() const noexcept { return m_cap; }

    void reserve(size_t count) {
        if (count <= m_cap) return;

        count = std::max(count, m_cap * 2);

        auto old_data = m_data;
        auto old_cap = m_cap;

        m_data = m_alloc.allocate(count);
        m_cap = count;

        if (old_cap == 0)
            return;

        for (size_t i = 0 ; i < m_size ; i ++) {
            std::construct_at(&m_data[i], std::as_const(old_data[i]));
        }
        m_alloc.deallocate(old_data, old_cap);
    }

    void shrink_to_fit() {
        if (m_cap == m_size)
            return;
        if (m_cap == 0)
            return;

        auto old_data = m_data;
        auto old_cap = m_cap;
        m_cap = m_size;

        if (m_size == 0) {
            m_data = nullptr;
        } else {
            m_data = m_alloc.allocate(m_size);
            for (size_t i = 0 ; i < m_size ; i ++) {
                std::construct_at(&m_data[i], std::as_const(old_data[i]));
            }
        }

        m_alloc.deallocate(old_data, old_cap);
    }

    T* begin() noexcept { return m_data; }
    const T* begin() const noexcept { return m_data; }
    const T* cbegin() const noexcept { return m_data; }
    std::reverse_iterator<T*> rbegin() noexcept { return std::make_reverse_iterator(end()); }
    const std::reverse_iterator<T*> rbegin() const noexcept { return std::make_reverse_iterator(end()); }
    const std::reverse_iterator<T*> rcbegin() const noexcept { return std::make_reverse_iterator(end()); }

    T* end() noexcept { return m_data + m_size; }
    const T* end() const noexcept { return m_data + m_size; }
    const T* cend() const noexcept { return m_data + m_size; }
    std::reverse_iterator<T*> rend() noexcept { return std::make_reverse_iterator(begin()); }
    const std::reverse_iterator<T*> rend() const noexcept { return std::make_reverse_iterator(begin()); }
    const std::reverse_iterator<T*> rcend() const noexcept { return std::make_reverse_iterator(begin()); }

    T* data() noexcept { return m_data; };
    const T* data() const noexcept { return m_data; };

    T& front() noexcept { return m_data[0]; };
    const T& front() const noexcept { return m_data[0]; };

    T& back() noexcept { return m_data[m_size - 1]; };
    const T& back() const noexcept { return m_data[m_size - 1]; };

    T& at(size_t pos) {
        if (pos > m_size)
            throw std::out_of_range("");
        return m_data[pos];
    }
    const T& at(size_t pos) const {
        if (pos > m_size)
            throw std::out_of_range("");
        return m_data[pos];
    }

    T& operator[](size_t pos) noexcept { return m_data[pos]; };
    const T& operator[](size_t pos) const noexcept { return m_data[pos]; };
};

int main() {

    struct A {
        A(int x) : x(x) {
            std::cout << __PRETTY_FUNCTION__ << "\n";
        }
        A(const A& that) : x(that.x) {
            std::cout << __PRETTY_FUNCTION__ << "\n";
        }
        A(A&& that) : x(that.x) {
            std::cout << __PRETTY_FUNCTION__ << "\n";
        }
        int x;
    };


    Vector<A> v;
    v.assign(10, A(3));
    v.push_back(3);

    for (auto it = v.rbegin(); it != v.rend(); it++) {
        std::cout << it->x << '\n';
    }
}
