#include <atomic>
#include <memory>
#include <type_traits>
#include <iostream>

struct _SpCounter {
    std::atomic<long> _M_refcnt;

    explicit _SpCounter() noexcept : _M_refcnt(1) {}
    _SpCounter(_SpCounter&&) = delete;
    virtual ~_SpCounter() = default;

    void incref() noexcept { _M_refcnt.fetch_add(1); }

    void decref() noexcept {
        if (_M_refcnt.fetch_sub(1) == 1) {
            delete this;
        }
    }

    long cntref() noexcept { return _M_refcnt.load(); }
};

template<typename T, typename Deleter>
struct _SpCounterImpl : _SpCounter {
    T* _M_ptr;
    Deleter _M_deleter;

    explicit _SpCounterImpl(T* ptr) noexcept : _M_ptr(ptr) {}
    explicit _SpCounterImpl(T* ptr, Deleter deleter) noexcept : _M_ptr(ptr), _M_deleter(std::move(deleter)) {}

    ~_SpCounterImpl() noexcept override { _M_deleter(_M_ptr); }
};

template<typename T>
struct DefaultDeleter {
    void operator()(T* p) noexcept {
        delete p;
    }
};

template <class>
class SharedPointer;

template <typename Derived>
class EnableSharedFromThis {
private:
    _SpCounter* _M_owner;

protected:
    EnableSharedFromThis() noexcept : _M_owner(nullptr) {}

    SharedPointer<Derived> shared_from_this() {
        static_assert(std::is_base_of_v<EnableSharedFromThis, Derived>, "must be a derived class");

        if (!_M_owner)
            throw std::bad_weak_ptr();
        _M_owner->incref();
        return _S_makeSharedFused(static_cast<Derived*>(this), _M_owner);
    }

    SharedPointer<const Derived> shared_from_this() const {
        static_assert(std::is_base_of_v<EnableSharedFromThis, Derived>, "must be a derived class");

        if (!_M_owner)
            throw std::bad_weak_ptr();
        _M_owner->incref();
        return _S_makeSharedFused(static_cast<const Derived*>(this), _M_owner);
    }

    template<typename T>
    inline friend void _S_setEnableSharedFromThisOwner(EnableSharedFromThis<T> *ptr, _SpCounter *owner) noexcept;
};

template<typename T>
inline void _S_setEnableSharedFromThisOwner(EnableSharedFromThis<T> *ptr, _SpCounter *owner) noexcept {
    ptr->_M_owner = owner;
}

template<typename T, std::enable_if_t<std::is_base_of_v<EnableSharedFromThis<T>, T>, int> = 0>
void _S_setEnableSharedFromThis(T *ptr, _SpCounter *owner) noexcept {
    _S_setEnableSharedFromThisOwner(static_cast<EnableSharedFromThis<T>*>(ptr), owner);
}

template<typename T, std::enable_if_t<!std::is_base_of_v<EnableSharedFromThis<T>, T>, int> = 0>
void _S_setEnableSharedFromThis(T *ptr, _SpCounter *owner) noexcept {
    (void)ptr;
    (void)owner;
}

template<typename T>
class SharedPointer {
private:
    T* _M_ptr;
    _SpCounter *_M_owner;

    template<class>
    friend class SharedPointer;

    explicit SharedPointer(T* ptr, _SpCounter* owner) : _M_ptr(ptr), _M_owner(owner) {}

public:
  SharedPointer(std::nullptr_t = nullptr) noexcept : _M_ptr(nullptr), _M_owner(nullptr) {}

  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer(Y* ptr) : _M_ptr(ptr), _M_owner(new _SpCounterImpl<T, DefaultDeleter<Y>>(ptr)) {
      _S_setEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template<typename Y, typename Deleter, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer(Y* ptr, Deleter deleter) : _M_ptr(ptr), _M_owner(new _SpCounterImpl<T, Deleter>(ptr, std::move(deleter))) {
      _S_setEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template<class Y>
  inline friend SharedPointer<Y> _S_makeSharedFused(Y* ptr, _SpCounter* owner) noexcept;

  SharedPointer(const SharedPointer& that) noexcept : _M_ptr(that._M_ptr), _M_owner(that._M_owner) {
      if (_M_owner)
          _M_owner->incref();
  }
  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer(const SharedPointer<Y>& that) noexcept : _M_ptr(that._M_ptr), _M_owner(that._M_owner) {
      if (_M_owner)
          _M_owner->incref();
  }
  template<typename Y>
  SharedPointer(const SharedPointer<Y>& that, T* ptr) noexcept : _M_ptr(ptr), _M_owner(that._M_owner) {
      if (_M_owner)
          _M_owner->incref();
  }

  ~SharedPointer() noexcept {
      if (_M_owner)
          _M_owner->decref();
  }

  SharedPointer& operator=(const SharedPointer& that) noexcept {
      if (this == &that) return *this;

      if (_M_owner)
          _M_owner->decref();
      _M_ptr = that._M_ptr;
      _M_owner = that._M_owner;
      if (_M_owner)
          _M_owner->incref();

      return *this;
  }
  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer& operator=(const SharedPointer<Y>& that) noexcept {
      if (this == &that) return *this;

      if (_M_owner)
          _M_owner->decref();
      _M_ptr = that._M_ptr;
      _M_owner = that._M_owner;
      if (_M_owner)
          _M_owner->incref();

      return *this;
  }

  SharedPointer(SharedPointer&& that) noexcept : _M_ptr(that._M_ptr), _M_owner(that._M_owner) {
      that._M_ptr = nullptr;
      that._M_owner = nullptr;
  }
  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer(SharedPointer<Y>&& that) noexcept : _M_ptr(that._M_ptr), _M_owner(that._M_owner) {
      that._M_ptr = nullptr;
      that._M_owner = nullptr;
  }
  template<typename Y>
  SharedPointer(SharedPointer<Y>&& that, T* ptr) noexcept : _M_ptr(ptr), _M_owner(that._M_owner) {
      that._M_ptr = nullptr;
      that._M_owner = nullptr;
  }

  SharedPointer& operator=(SharedPointer&& that) noexcept {
      if (this == &that) return *this;

      if (_M_owner)
          _M_owner->decref();
      _M_ptr = that._M_ptr;
      _M_owner = that._M_owner;
      that._M_ptr = nullptr;
      that._M_owner = nullptr;

      return *this;
  }
  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  SharedPointer& operator=(SharedPointer<Y>&& that) noexcept {
      if (this == &that) return *this;

      if (_M_owner)
          _M_owner->decref();
      _M_ptr = that._M_ptr;
      _M_owner = that._M_owner;
      that._M_ptr = nullptr;
      that._M_owner = nullptr;

      return *this;
  }

  void reset() noexcept {
      _M_ptr = nullptr;
      if (_M_owner)
          _M_owner->decref();
      _M_owner = nullptr;
  }

  template<typename Y, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  void reset(Y* ptr) {
      _M_ptr = ptr;
      if (_M_owner)
          _M_owner->decref();
      _M_owner = new _SpCounterImpl<Y, DefaultDeleter<Y>>(ptr);
      _S_setEnableSharedFromThis(_M_ptr, _M_owner);
  }

  template<typename Y, typename Deleter, std::enable_if_t<std::is_convertible_v<Y*, T*>, int> = 0>
  void reset(Y* ptr, Deleter deleter) {
      _M_ptr = ptr;
      if (_M_owner)
          _M_owner->decref();
      _M_owner = new _SpCounterImpl<Y, Deleter>(ptr, std::move(deleter));
      _S_setEnableSharedFromThis(_M_ptr, _M_owner);
  }

  long use_count() const noexcept {
      return _M_owner ? _M_owner->cntref() : 0;
  }

  bool unique() const noexcept {
      return _M_owner ? _M_owner->cntref() == 1: true;
  }

  T* get() const noexcept { return _M_ptr; }
  T* operator*() const noexcept { return _M_ptr; }
  T* operator->() const noexcept { return _M_ptr; }

  explicit operator bool() const noexcept {
      return _M_ptr != nullptr;
  }
};

template<typename T, typename ...Args>
SharedPointer<T> makeShared(Args && ...args) {
    return SharedPointer<T>(new T(std::forward<Args>(args)...));
}

template<typename Y>
inline SharedPointer<Y> _S_makeSharedFused(Y* ptr, _SpCounter* owner) noexcept {
    return SharedPointer(ptr, owner);
}


struct B : EnableSharedFromThis<B> {
    SharedPointer<B> getptr() {
        return shared_from_this();
    }
};

int main() {
    SharedPointer<B> p = makeShared<B>();
    SharedPointer<const B> cp = p.get()->getptr();
    std::cout << p.use_count() << "\n";
}
