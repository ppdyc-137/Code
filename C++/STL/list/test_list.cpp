#include "list.hpp"

#include <cassert>
#include <iostream>

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

    int x{};
    inline static int count{0};
};

template <typename T>
void print(T const& v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        std::cout << it->x << ' ';
    }
    std::cout << '\n';
}

int main() {
    {
        List<A> l = {1, 2, 3, 4};

        l.emplace_back(5);
        print(l);
        assert(A::count == 5);

        l.emplace_front(0);
        print(l);
        assert(A::count == 6);

        l.pop_front();
        print(l);
        assert(A::count == 5);

        l.erase(l.erase(l.begin()));
        print(l);
        assert(A::count == 3);

        l.erase(l.insert(l.end(), {1, 2, 3}));
        print(l);
        assert(A::count == 5);

        l.resize(10);
        print(l);
        assert(A::count == 10);

        {
            auto l2 = std::move(l);
            assert(A::count == 10);

            l = std::move(l2);
            assert(A::count == 10);
        }

        {
            auto l2 = l;
            assert(A::count == 20);

            auto& l3 = l2;
            l2 = l3;
            assert(A::count == 20);
        }

        l = {5, 4, 3, 2, 1};
        print(l);
        assert(A::count == 5);

        // List<A> l2 = {1, 2};
        // l.splice(l.end(), std::move(l2));
        // print(l2);
        // assert(A::count == 7);
    }
    assert(A::count == 0);
}
