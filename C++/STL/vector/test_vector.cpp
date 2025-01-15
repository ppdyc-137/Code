#include "vector.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

// #define DEBUG

struct A {
    A() {
        count++;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
    }
    A(int x) : x(x) {
        count++;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
    }
    A(const A& that) : x(that.x) {
        count++;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
    }
    A(A&& that) : x(that.x) {
        count++;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
    }
    A& operator=(A&& that) {
        if (this == &that)
            return *this;
        x = that.x;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
        return *this;
    }
    A& operator=(A const& that) {
        x = that.x;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
        return *this;
    }
    ~A() {
        count--;
#ifdef DEBUG
        std::cout << __PRETTY_FUNCTION__ << "\n";
#endif
    }

    int x;
    inline static int count{0};
};

template <typename T>
void print(T const& v) {
    for (auto it = v.rbegin(); it != v.rend(); it++) {
        std::cout << it->x << ' ';
    }
    std::cout << '\n';
}

template <typename T>
struct Allocator {
    T* allocate(std::size_t count) {
        auto data = static_cast<T*>(malloc(sizeof(T) * count));
#ifdef DEBUG
        std::cout << "  allocate: " << data << ' ' << count << '\n';
#endif
        return data;
    }
    void deallocate(T* data, std::size_t count) {
#ifdef DEBUG
        std::cout << "deallocate: " << data << ' ' << count << '\n';
#endif
        free(data);
    }
};

int main() {
    {
        Vector<A, Allocator<A>> v;
        // std::vector<A> v;
        v.push_back(4);
        assert(A::count == 1);

        v.insert(v.begin(), 0);
        assert(A::count == 2);

        v.erase(v.begin());
        assert(A::count == 1);

        v.clear();
        assert(A::count == 0);

        v.clear();
        assert(A::count == 0);

        v.emplace_back(5);
        assert(A::count == 1);

        v.shrink_to_fit();
        assert(A::count == 1);

        v.insert(v.begin(), 3, 1);
        assert(A::count == 4);

        v.assign(10, 1);
        assert(A::count == 10);

        auto v2 = v;
        assert(A::count == 20);

        v2.insert(v2.begin(), v.begin(), v.end());
        assert(A::count == 30);

        v2 = v;
        assert(A::count == 20);

        v2 = std::move(v);
        assert(A::count == 10);

        v2.resize(5);
        assert(A::count == 5);

        print(v2);
    }
    assert(A::count == 0);
}
