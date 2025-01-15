#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <memory>
#include <utility>

template <typename T, typename Alloc = std::allocator<T>>
class Vector {
private:
    T* data_{};
    size_t size_{};
    size_t cap_{};

    [[no_unique_address]] Alloc alloc_{};

public:
    using value_type = T;
    using iterator = value_type*;
    using const_iterator = value_type const*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Vector() = default;

    explicit Vector(const Alloc& alloc) : alloc_(alloc) {}
    explicit Vector(size_t count, const Alloc& alloc = Alloc()) : size_(count), cap_(count), alloc_(alloc) {
        data_ = alloc_.allocate(count);
        for (size_t i = 0; i < count; i++) {
            std::construct_at(&data_[i], T());
        }
    }

    Vector(size_t count, const T& value, const Alloc& alloc = Alloc()) : size_(count), cap_(count), alloc_(alloc) {
        data_ = alloc_.allocate(count);
        for (size_t i = 0; i < count; i++) {
            std::construct_at(&data_[i], std::as_const(value));
        }
    }

    template <typename InputIt>
    Vector(InputIt first, InputIt last, const Alloc& alloc = Alloc()) : alloc_(alloc) {
        auto distance = last - first;
        size_ = distance;
        cap_ = distance;
        data_ = alloc_.allocate(distance);
        for (size_t i = 0; i < distance; i++) {
            std::construct_at(&data_[i], std::as_const(*(first + i)));
        }
    }

    Vector(std::initializer_list<T> init) : Vector(init.begin(), init.end()) {}

    ~Vector() {
        if (cap_) {
            clear();
            alloc_.deallocate(data_, cap_);
        }
    }

    Vector(const Vector& that) {
        size_ = that.size_;
        cap_ = that.cap_;
        alloc_ = that.alloc_;
        if (size_ == 0) {
            data_ = nullptr;
        } else {
            data_ = alloc_.allocate(size_);
            for (size_t i = 0; i < size_; i++) {
                std::construct_at(&data_[i], std::as_const(that.data_[i]));
            }
        }
    }
    Vector& operator=(const Vector& that) {
        if (data_) {
            clear();
            alloc_.deallocate(data_, cap_);
        }
        size_ = that.size_;
        cap_ = that.cap_;
        alloc_ = that.alloc_;

        if (size_ == 0) {
            data_ = nullptr;
        } else {
            data_ = alloc_.allocate(size_);
            for (size_t i = 0; i < size_; i++) {
                std::construct_at(&data_[i], std::as_const(that.data_[i]));
            }
        }
        return *this;
    }

    Vector(Vector&& that) noexcept {
        data_ = std::exchange(that.data_, nullptr);
        size_ = std::exchange(that.size_, 0);
        cap_ = std::exchange(that.cap_, 0);
        alloc_ = that.alloc_;
    }
    Vector& operator=(Vector&& that) noexcept {
        if (this == &that)
            return *this;

        if (data_) {
            clear();
            alloc_.deallocate(data_, cap_);
        }
        data_ = std::exchange(that.data_, nullptr);
        size_ = std::exchange(that.size_, 0);
        cap_ = std::exchange(that.cap_, 0);
        alloc_ = that.alloc_;
        return *this;
    }

    void assign(size_t count, const T& value) {
        resize(count);
        for (size_t i = 0; i < count; i++) {
            data_[i] = value;
        }
    }
    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        size_t count = std::distance(first, last);
        resize(count);
        for (size_t i = 0; i < count; i++) {
            data_[i] = std::as_const(*(first + i));
        }
    }
    void assign(std::initializer_list<T> init) { assign(init.begin(), init.end()); }

    void clear() noexcept {
        for (std::size_t i = 0; i < size_; i++) {
            std::destroy_at(&data_[i]);
        }
        size_ = 0;
    }

    iterator insert(const_iterator pos, const T& value) { return insert(pos, 1, value); }
    iterator insert(const_iterator pos, T&& value) { return insert(pos, 1, std::move(value)); }
    iterator insert(const_iterator pos, std::size_t count, T&& value) {
        if (count == 0) {
            return const_cast<iterator>(pos);
        }
        std::size_t index = static_cast<std::size_t>(std::distance(cbegin(), pos));
        resize(size_ + count);
        for (auto i = size_ - 1; i >= index + count; i--) {
            data_[i] = std::move(data_[i - count]);
        }
        for (std::size_t i = 0; i < count; i++) {
            data_[index + i] = std::forward<T>(value);
        }
        return &data_[index];
    }
    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto count = std::distance(first, last);
        if (count == 0) {
            return const_cast<iterator>(pos);
        }
        std::size_t index = static_cast<std::size_t>(std::distance(cbegin(), pos));
        resize(size_ + count);
        for (auto i = size_ - 1; i >= index + count; i--) {
            data_[i] = std::move(data_[i - count]);
        }
        for (std::size_t i = 0; i < count; i++) {
            data_[index + i] = *(first + i);
        }
        return &data_[index];
    }
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) { insert(pos, ilist.begin(), ilist.end()); }

    T* erase(const T* _pos) {
        auto pos = const_cast<T*>(_pos);

        if (pos == end())
            return end();

        for (auto it = pos; it + 1 != end(); it++) {
            *it = std::move(*(it + 1));
        }
        resize(size_ - 1);
        return pos;
    }
    T* erase(const T* _first, const T* _last) {
        if (_first == _last)
            return const_cast<T*>(_last);

        auto first = const_cast<T*>(_first);
        auto last = const_cast<T*>(_last);
        auto distance = std::distance(first, last);
        for (auto it = first; it + distance != end(); it++) {
            *it = std::move(*(it + distance));
        }
        resize(size_ - distance);
        return first;
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    template <typename... Args>
    T& emplace_back(Args&&... args) {
        reserve(size_ + 1);
        std::construct_at(end(), std::forward<Args>(args)...);
        size_++;
        return back();
    }

    void resize(size_t count, const T& value = T()) {
        reserve(count);

        if (size_ < count) {
            for (auto i = size_; i < count; i++) {
                std::construct_at(&data_[i], value);
            }
        } else {
            for (auto i = count; i < size_; i++) {
                std::destroy_at(&data_[i]);
            }
        }
        size_ = count;
    }

    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }
    size_t capacity() const noexcept { return cap_; }

    void reserve(size_t count) {
        if (count <= cap_)
            return;

        count = std::max(count, cap_ * 2);

        auto old_data = data_;
        auto old_cap = cap_;
        data_ = alloc_.allocate(count);
        cap_ = count;

        if (old_cap == 0)
            return;

        for (size_t i = 0; i < size_; i++) {
            std::construct_at(&data_[i], std::as_const(old_data[i]));
        }
        for (size_t i = 0; i < size_; i++) {
            std::destroy_at(&old_data[i]);
        }
        alloc_.deallocate(old_data, old_cap);
    }

    void shrink_to_fit() {
        if (cap_ == size_ || cap_ == 0)
            return;

        auto old_data = data_;
        auto old_cap = cap_;
        cap_ = size_;

        if (size_ == 0) {
            data_ = nullptr;
        } else {
            data_ = alloc_.allocate(size_);
            for (size_t i = 0; i < size_; i++) {
                std::construct_at(&data_[i], std::as_const(old_data[i]));
            }
        }

        for (size_t i = 0; i < size_; i++) {
            std::destroy_at(&old_data[i]);
        }
        alloc_.deallocate(old_data, old_cap);
    }

    iterator begin() noexcept { return data_; }
    const_iterator begin() const noexcept { return data_; }
    const_iterator cbegin() const noexcept { return data_; }
    reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(end()); }
    const_reverse_iterator rcbegin() const noexcept { return std::make_reverse_iterator(end()); }

    iterator end() noexcept { return data_ + size_; }
    const_iterator end() const noexcept { return data_ + size_; }
    const_iterator cend() const noexcept { return data_ + size_; }
    reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator rcend() const noexcept { return std::make_reverse_iterator(begin()); }

    T* data() noexcept { return data_; };
    const T* data() const noexcept { return data_; };

    T& front() noexcept { return data_[0]; };
    const T& front() const noexcept { return data_[0]; };

    T& back() noexcept { return data_[size_ - 1]; };
    const T& back() const noexcept { return data_[size_ - 1]; };

    T& at(size_t pos) {
        if (pos > size_)
            throw std::out_of_range("out of range");
        return data_[pos];
    }
    const T& at(size_t pos) const {
        if (pos > size_)
            throw std::out_of_range("out of range");
        return data_[pos];
    }

    T& operator[](size_t pos) noexcept { return data_[pos]; };
    const T& operator[](size_t pos) const noexcept { return data_[pos]; };
};
