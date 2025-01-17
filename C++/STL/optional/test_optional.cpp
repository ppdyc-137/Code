#include "optional.hpp"
#include <cassert>
#include <string>

void test_basic_operations() {
    // Default construction
    Optional<int> opt1;
    assert(!opt1.has_value());

    // Construction with value
    Optional<int> opt2(42);
    assert(opt2.has_value());
    assert(opt2.value() == 42);

    // Copy construction
    Optional<int> opt3(opt2);
    assert(opt3.has_value());
    assert(opt3.value() == 42);

    // Move construction
    Optional<std::string> opt4("test");
    Optional<std::string> opt5(std::move(opt4));
    assert(opt5.has_value());
    assert(opt5.value() == "test");
}

void test_value_access() {
    Optional<int> opt1(42);
    assert(opt1.value() == 42);

    Optional<int> opt2;
    assert(opt2.value_or(100) == 100);

    bool exception_thrown = false;
    try {
        opt2.value();
    } catch (const BadOptionalAccess&) {
        exception_thrown = true;
    }
    assert(exception_thrown);
}

void test_non_trivial_type() {
    struct Test {
        std::string str;
        int value;
        Test(std::string s, int v) : str(std::move(s)), value(v) {}
    };

    Optional<Test> opt1(inplace, "test", 42);
    assert(opt1.has_value());
    assert(opt1->str == "test");
    assert(opt1->value == 42);

    Optional<Test> opt2;
    opt2.emplace("other", 100);
    assert(opt2.has_value());
    assert(opt2->str == "other");
    assert(opt2->value == 100);
}

void test_operators() {
    Optional<int> opt1(42);
    assert(*opt1 == 42);

    struct Test {
        int x;
    };
    Optional<Test> opt2(inplace, 42);
    assert(opt2->x == 42);

    Optional<int> opt3;
    assert(!opt3);
    assert(bool(opt1));
}

void test_reset_and_emplace() {
    Optional<int> opt1(42);
    opt1.reset();
    assert(!opt1.has_value());

    opt1.emplace(100);
    assert(opt1.has_value());
    assert(opt1.value() == 100);
}

int main() {
    test_basic_operations();
    test_value_access();
    test_non_trivial_type();
    test_operators();
    test_reset_and_emplace();
    return 0;
}
