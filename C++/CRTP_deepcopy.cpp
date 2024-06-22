#include <iostream>
#include <memory>

using namespace std;

class Base {
public:
    virtual void foo() {
        cout << "Base::foo()" << endl;
    }

    virtual unique_ptr<Base> clone() = 0;
};

template <typename Derived>
class BaseImpl : public Base {
public:
    unique_ptr<Base> clone() override {
        return make_unique<Derived>(*static_cast<Derived*>(this));
    }
};

class Derived : public BaseImpl<Derived> {
public:
    void foo() override {
        cout << "Derived::foo()" << endl;
    }
};

int main() {
    Base* derived = new Derived();
    derived->foo();

    auto A = derived->clone();
    A->foo();

    return 0;
}
