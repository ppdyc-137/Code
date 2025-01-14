#include <iostream>
#include <type_traits>
#include <utility>

template<typename T>
struct DefaultDeleter {
    void operator()(T* ptr) {
        delete ptr;
    }
};

template<typename T>
struct DefaultDeleter<T[]> {
    void operator()(T* ptr) {
        delete[] ptr;
    }
};

template<typename T, typename Deleter = DefaultDeleter<T>>
class UniquePtr {
private:
    T* m_ptr{nullptr};

    template<class , class>
    friend class UniquePtr;

public:
    UniquePtr() {}
    UniquePtr(T* ptr) : m_ptr(ptr) {}

    template<typename Y, typename YDeleter, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
    UniquePtr(UniquePtr<Y, YDeleter>&& that) : m_ptr(std::exchange(that.m_ptr, nullptr)) {}

    ~UniquePtr() {
        if (m_ptr)
            Deleter{}(m_ptr);
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    UniquePtr(UniquePtr&& that) {
        if (this == &that)
            return;
        if (m_ptr)
            Deleter{}(m_ptr);
        m_ptr = std::exchange(that.m_ptr, nullptr);
    }

    UniquePtr& operator=(UniquePtr&& that) {
        if (this == &that)
            return *this;
        if (m_ptr)
            Deleter{}(m_ptr);
        m_ptr = std::exchange(that.m_ptr, nullptr);
    }

    T* get() const {
        return m_ptr;
    }

    T* operator->() const {
        return m_ptr;
    }

    T& operator*() const {
        return *m_ptr;
    }

    T* release() {
        return std::exchange(m_ptr, nullptr);
    }

    void reset(T* p = nullptr) {
        if (m_ptr)
            Deleter{}(m_ptr);
        m_ptr = p;
    }
};

template<typename T, typename ...Args>
UniquePtr<T> makeUnique(Args&& ...args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

template<typename T, typename ...Args>
UniquePtr<T> makeUniqueForOverwrrte(Args&& ...args) {
    (void)sizeof...(args);
    return UniquePtr<T>(new T);
}

struct A {
    virtual ~A() {
        std::cout << __PRETTY_FUNCTION__ << "\n";
    }
    int a;
};

struct B : A {
};

int main() {
    UniquePtr<A> a = makeUnique<B>();
    UniquePtr b = std::move(a);
    std::cout << b->a << '\n';
}
