#include <iostream>
#include <vector>

template<class T>
void func(T& t) {
    std::cout << "lvalue\n";
}

template<class T>
void func(T&& t) {
    std::cout << "rvalue\n";
}

template<class T>
void push_back(T&& t) {
    auto _t = new T(std::forward<T>(t));
    std::cout << "push_back\n";
}

template<class T, class ...Args>
void emplace_back(Args... args) {
    auto _t = new T(std::forward<Args>(args)...);
    std::cout << "emplace_back\n";
}

struct A {
    A(int a){
        std::cout << "A()\n";
    }
    A(const A& a) {
        std::cout << "A(&)\n";
    }
    A(A&& a) {
        std::cout << "A(&&)\n";
    }
};

int main() {
    std::vector<A> v;
    v.push_back(A(1));
    v.emplace_back(1);

    push_back(A(1));
    emplace_back<A>(1);
}
