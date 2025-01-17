#pragma once

#include <exception>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

struct BadOptionalAccess : std::exception {
    BadOptionalAccess() = default;
    virtual ~BadOptionalAccess() = default;

    const char* what() const noexcept override { return "BadOptionalAccess"; }
};

struct NullOpt {
    explicit NullOpt() = default;
};

inline constexpr NullOpt nullopt;

struct InPlace {
    explicit InPlace() = default;
};
inline constexpr InPlace inplace;

template <typename T>
class Optional {

public:
    constexpr Optional() noexcept {}
    constexpr Optional(NullOpt) noexcept {};
    constexpr Optional(Optional const& other) { _uninit_construct(other); }
    constexpr Optional(Optional&& other) { _uninit_construct(std::move(other)); }
    constexpr Optional(T&& value) : value_{std::move(value)}, has_value_(true) {}
    template <typename... Args>
    constexpr explicit Optional(InPlace, Args&&... args) : value_(std::forward<Args>(args)...), has_value_(true) {}

    Optional& operator=(NullOpt) noexcept {
        reset();
        return *this;
    }
    constexpr Optional& operator=(Optional const& other) {
        if (this == &other) {
            return *this;
        }
        reset();
        _uninit_construct(other);
        return *this;
    }
    constexpr Optional& operator=(Optional&& other) {
        if (this == &other) {
            return *this;
        }
        reset();
        _uninit_construct(std::move(other));
        return *this;
    }
    Optional& operator=(T&& value) {
        emplace(std::move(value));
        return *this;
    }

    ~Optional() {
        if (has_value_) {
            std::destroy_at(&value_);
        }
    }

    void reset() noexcept {
        if (has_value_) {
            std::destroy_at(&value_);
        }
        has_value_ = false;
    }

    template <typename... Args>
    T& emplace(Args&&... args) {
        reset();
        std::construct_at(&value_, std::forward<Args>(args)...);
        has_value_ = true;
        return value_;
    }

    template <typename F>
    constexpr auto and_then(F f) & {
        if (has_value_) {
            return std::invoke(std::forward<F>(f), value_);
        } else {
            using RetType = std::remove_cvref_t<std::invoke_result_t<F, T&>>;
            return RetType{};
        }
    }
    template <typename F>
    constexpr auto and_then(F f) const& {
        if (has_value_) {
            return std::invoke(std::forward<F>(f), value_);
        } else {
            using RetType = std::remove_cvref_t<std::invoke_result_t<F, T const&>>;
            return RetType{};
        }
    }
    template <typename F>
    constexpr auto and_then(F f) && {
        if (has_value_) {
            return std::invoke(std::forward<F>(f), std::move(value_));
        } else {
            using RetType = std::remove_cvref_t<std::invoke_result_t<F, T>>;
            return RetType{};
        }
    }
    template <typename F>
    constexpr auto and_then(F f) const&& {
        if (has_value_) {
            return std::invoke(std::forward<F>(f), std::move(value_));
        } else {
            using RetType = std::remove_cvref_t<std::invoke_result_t<F, T const>>;
            return RetType{};
        }
    }

    template <class F>
    constexpr auto transform(F&& f) & -> Optional<std::remove_cvref_t<std::invoke_result_t<F, T&>>>;
    template <class F>
    constexpr auto transform(F&& f) const& -> Optional<std::remove_cvref_t<std::invoke_result_t<F, T const&>>>;
    template <class F>
    constexpr auto transform(F&& f) && -> Optional<std::remove_cvref_t<std::invoke_result_t<F, T>>>;
    template <class F>
    constexpr auto transform(F&& f) const&& -> Optional<std::remove_cvref_t<std::invoke_result_t<F, T const>>>;

    template <class F>
    constexpr Optional or_else(F&& f) const&;
    template <class F>
    constexpr Optional or_else(F&& f) &&;

    constexpr T& value() & {
        if (!has_value_) {
            throw BadOptionalAccess{};
        }
        return value_;
    }
    constexpr T const& value() const& {
        if (!has_value_) {
            throw BadOptionalAccess{};
        }
        return value_;
    }
    constexpr T&& value() && {
        if (!has_value_) {
            throw BadOptionalAccess{};
        }
        return std::move(value_);
    }
    constexpr T const && value() const&& {
        if (!has_value_) {
            throw BadOptionalAccess{};
        }
        return std::move(value_);
    }

    template <typename U>
    constexpr T value_or(U&& default_value) const& noexcept {
        if (!has_value_) {
            return std::forward<U>(default_value);
        }
        return value_;
    }
    template <typename U>
    constexpr T value_or(U&& default_value) && noexcept {
        if (!has_value_) {
            return std::forward<U>(default_value);
        }
        return std::move(value_);
    }

    constexpr T* operator->() noexcept { return &value_; }
    constexpr T const* operator->() const noexcept { return &value_; }
    constexpr T& operator*() & noexcept { return value_; }
    constexpr T const& operator*() const& noexcept { return value_; }
    constexpr T&& operator*() && noexcept { return std::move(value_); }
    constexpr T const&& operator*() const&& noexcept { return std::move(value_); }

    constexpr bool has_value() const noexcept { return has_value_; }
    constexpr explicit operator bool() const noexcept { return has_value_; }

private:
    void _uninit_construct(Optional const& other) {
        if (other.has_value_) {
            std::construct_at(&value_, other.value_);
        }
        has_value_ = other.has_value_;
    }
    void _uninit_construct(Optional&& other) {
        if (other.has_value_) {
            std::construct_at(&value_, std::move(other.value_));
        }
        has_value_ = other.has_value_;
    }

    union {
        T value_;
        NullOpt nullopt_;
    };
    bool has_value_{false};
};

template <typename T, typename... Args>
inline Optional<T> makeOptional(Args&&... args) {
    return Optional<T>{inplace, std::forward<Args>(args)...};
}
