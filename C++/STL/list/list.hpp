#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <utility>

template <typename T, typename Allocator = std::allocator<T>>
class List {
    struct ListNode {
        ListNode* prev_;
        ListNode* next_;

        T& value() & { return static_cast<ListValueNode*>(this)->val_; }
        T const& value() const& { return static_cast<ListValueNode const*>(this)->val_; }
        // T&& value() && { return std::move(static_cast<ListValueNode const*>(this)->val_); }
    };

    struct ListValueNode : ListNode {
        T val_;
    };

    using AllocNode = std::allocator_traits<Allocator>::template rebind_alloc<ListValueNode>;
    ListNode* allocNode() { return AllocNode{alloc_}.allocate(1); }
    void deallocNode(ListNode* node) noexcept { AllocNode{alloc_}.deallocate(static_cast<ListValueNode*>(node), 1); }

public:
    List() noexcept : alloc_(Allocator()) {}
    explicit List(Allocator const& alloc) noexcept : alloc_(alloc) {}
    explicit List(std::size_t count, Allocator const& alloc = Allocator()) : alloc_(alloc) { assign(count, T()); }
    List(std::size_t count, T const& value, Allocator const& alloc = Allocator()) : alloc_(alloc) {
        assign(count, value);
    }

    template <std::input_iterator InputIt>
    List(InputIt first, InputIt last, Allocator const& alloc = Allocator()) : alloc_(alloc) {
        assign(first, last);
    }
    List(List const& other) : List(other.cbegin(), other.cend(), other.alloc_) {}
    List(List const& other, Allocator const& alloc) : List(other.cbegin(), other.cend(), alloc) {}
    List(List&& other) noexcept : alloc_(std::move(other.alloc_)) {
        if (other.empty()) {
            return;
        }
        _move_dummy(std::move(other));
    }
    List(List&& other, Allocator const* alloc) noexcept : alloc_(alloc) {
        if (other.empty()) {
            return;
        }
        _move_dummy(std::move(other));
    }
    List(std::initializer_list<T> init, Allocator const& alloc = Allocator()) : List(init.begin(), init.end(), alloc) {}

    List& operator=(List const& other) {
        if (this == &other) {
            return *this;
        }
        auto old_alloc = alloc_;
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            clear();
            alloc_ = other.alloc_;
            _uninit_assign(other.begin(), other.end());
        } else {
            assign(other.begin(), other.end());
        }
        return *this;
    }
    List& operator=(List&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        clear();
        if (other.empty()) {
            return *this;
        }

        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            alloc_ = other.alloc_;
            _move_dummy(std::move(other));
        } else if (alloc_ == other.alloc_) {
            _move_dummy(std::move(other));
        } else {
            _uninit_assign(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()));
            other.clear();
        }

        return *this;
    }
    List& operator=(std::initializer_list<T> ilist) {
        assign(ilist.begin(), ilist.end());
        return *this;
    }

    ~List() { clear(); }

    void clear() noexcept {
        for (auto* node = dummy_.next_; node != &dummy_;) {
            std::destroy_at(&node->value());
            node = node->next_;
            deallocNode(node->prev_);
        }
        dummy_.prev_ = dummy_.next_ = &dummy_;
    }

    void assign(std::size_t count, T const& value) {
        clear();
        _uninit_assign(count, value);
    }

    template <std::input_iterator InputIt>
    void assign(InputIt first, InputIt last) {
        clear();
        _uninit_assign(first, last);
    }

    void pop_back() { erase(std::prev(end())); }
    void push_back(T const& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }
    template <typename... Args>
    T& emplace_back(Args&&... args) {
        return *emplace(end(), std::forward<Args>(args)...);
    }

    void pop_front() { erase(begin()); }
    void push_front(T const& value) { emplace_front(value); }
    void push_front(T&& value) { emplace_front(std::move(value)); }
    template <typename... Args>
    T& emplace_front(Args&&... args) {
        return *emplace(begin(), std::forward<Args>(args)...);
    }

    void resize(std::size_t count) { resize(count, T()); }
    void resize(std::size_t count, T const& value) {
        auto msize = size();
        if (count == msize) {
            return;
        }
        if (count < msize) {
            erase(std::next(begin(), count), end());
        } else {
            insert(end(), count - msize, value);
        }
    }

    struct iterator {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator() = default;

        iterator& operator++() noexcept {
            cur_ = cur_->next_;
            return *this;
        }
        iterator operator++(int) noexcept {
            auto ret = *this;
            cur_ = cur_->next_;
            return ret;
        }
        iterator& operator--() noexcept {
            cur_ = cur_->prev_;
            return *this;
        }
        iterator operator--(int) noexcept {
            auto ret = *this;
            cur_ = cur_->prev_;
            return ret;
        }
        bool operator==(iterator const& other) const noexcept { return this->cur_ == other.cur_; }
        bool operator!=(iterator const& other) const noexcept { return this->cur_ != other.cur_; }
        T& operator*() const noexcept { return cur_->value(); }
        T* operator->() noexcept { return &cur_->value(); }
        T const* operator->() const noexcept { return &cur_->value(); }

    private:
        friend List;

        explicit iterator(ListNode* node) noexcept : cur_(node) {}
        ListNode* cur_{};
    };

    struct const_iterator {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T const*;
        using reference = T const&;
        using iterator_category = std::bidirectional_iterator_tag;

        const_iterator() noexcept = default;
        const_iterator(iterator other) noexcept : cur_(other.cur_) {}

        const_iterator& operator++() noexcept {
            cur_ = cur_->next_;
            return *this;
        }
        const_iterator operator++(int) noexcept {
            auto ret = *this;
            cur_ = cur_->next_;
            return ret;
        }
        const_iterator& operator--() noexcept {
            cur_ = cur_->prev_;
            return *this;
        }
        const_iterator operator--(int) noexcept {
            auto ret = *this;
            cur_ = cur_->prev_;
            return ret;
        }
        bool operator==(const_iterator const& other) const noexcept { return this->cur_ == other.cur_; }
        bool operator!=(const_iterator const& other) const noexcept { return this->cur_ != other.cur_; }
        T const& operator*() const noexcept { return cur_->value(); }
        T const* operator->() const noexcept { return &cur_->value(); }

    private:
        friend List;

        explicit const_iterator(ListNode const* node) noexcept : cur_(node) {}
        ListNode const* cur_{};
    };

    template <typename... Args>
    iterator emplace(const_iterator _pos, Args&&... args) {
        iterator pos{const_cast<ListNode*>(_pos.cur_)};
        auto* node = allocNode();
        std::construct_at(&node->value(), std::forward<Args>(args)...);
        node->prev_ = pos.cur_->prev_;
        node->next_ = pos.cur_;
        pos.cur_->prev_->next_ = node;
        pos.cur_->prev_ = node;
        return iterator{node};
    }

    iterator insert(const_iterator pos, T const& value) { return emplace(pos, value); }
    iterator insert(const_iterator pos, T&& value) { return emplace(pos, std::move(value)); }
    iterator insert(const_iterator pos, std::size_t count, T const& value) {
        iterator it{static_cast<ListNode*>(pos.cur_->prev_)};
        for (std::size_t i = 0; i < count; i++) {
            emplace(pos, value);
        }
        return ++it;
    }
    template <std::input_iterator InputIt>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        iterator it{static_cast<ListNode*>(pos.cur_->prev_)};
        while (first != last) {
            emplace(pos, *first);
            ++first;
        }
        return ++it;
    }
    iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    void splice(const_iterator pos, List& other);
    void splice(const_iterator pos, List& other, const_iterator it);
    void splice(const_iterator pos, List& other, const_iterator first, const_iterator last);
    void splice(const_iterator pos, List&& other);
    void splice(const_iterator pos, List&& other, const_iterator it);
    void splice(const_iterator pos, List&& other, const_iterator first, const_iterator last);

    iterator erase(const_iterator pos) noexcept {
        auto* node = pos.cur_;
        node->prev_->next_ = node->next_;
        node->next_->prev_ = node->prev_;

        auto ret = pos.cur_->next_;

        std::destroy_at(&node->value());
        deallocNode(const_cast<ListNode*>(node));

        return iterator{ret};
    }
    iterator erase(const_iterator first, const_iterator last) noexcept {
        while (first != last) {
            erase(first++);
        }
        return iterator{const_cast<ListNode*>(last.cur_)};
    }

    T& front() noexcept { return dummy_.next_->value(); }
    T const& front() const noexcept { return dummy_.next_->value(); }
    T& back() noexcept { return dummy_.prev_->value(); }
    T const& back() const noexcept { return dummy_.prev_->value(); }

    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin() noexcept { return iterator{dummy_.next_}; }
    const_iterator begin() const noexcept { return const_iterator{dummy_.next_}; }
    const_iterator cbegin() const noexcept { return const_iterator{dummy_.next_}; }
    reverse_iterator rbegin() noexcept { return std::make_reverse_iterator(end()); }
    const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(cend()); }
    const_reverse_iterator crbegin() const noexcept { return std::make_reverse_iterator(cend()); }

    iterator end() noexcept { return iterator{&dummy_}; }
    const_iterator end() const noexcept { return const_iterator{&dummy_}; }
    const_iterator cend() const noexcept { return const_iterator{&dummy_}; }
    reverse_iterator rend() noexcept { return std::make_reverse_iterator(begin()); }
    const_reverse_iterator rend() const noexcept { return std::make_reverse_iterator(cbegin()); }
    const_reverse_iterator crend() const noexcept { return std::make_reverse_iterator(cbegin()); }

    bool empty() const noexcept { return begin() == end(); };
    std::size_t size() const noexcept { return std::distance(begin(), end()); };
    static constexpr std::size_t max_size() noexcept { return std::numeric_limits<std::size_t>::max(); }

private:
    void _move_dummy(List&& other) {
        dummy_.next_ = other.dummy_.next_;
        dummy_.next_->prev_ = &dummy_;
        dummy_.prev_ = other.dummy_.prev_;
        dummy_.prev_->next_ = &dummy_;
        other.dummy_.next_ = &other.dummy_;
        other.dummy_.prev_ = &other.dummy_;
    }

    void _uninit_assign(std::size_t count, T const& value) {
        clear();
        if (count == 0) {
            return;
        }

        auto* cur = &dummy_;
        for (std::size_t i = 0; i < count; i++) {
            auto* node = allocNode();
            std::construct_at(&node->value(), value);
            node->prev_ = cur;
            cur->next_ = node;
            cur = node;
        }
        cur->next_ = &dummy_;
        dummy_.prev_ = cur;
    }

    template <std::input_iterator InputIt>
    void _uninit_assign(InputIt first, InputIt last) {
        if (first == last) {
            return;
        }

        auto* cur = &dummy_;
        while (first != last) {
            auto* node = allocNode();
            std::construct_at(&node->value(), *first);
            ++first;
            node->prev_ = cur;
            cur->next_ = node;
            cur = node;
        }
        cur->next_ = &dummy_;
        dummy_.prev_ = cur;
    }

private:
    ListNode dummy_{&dummy_, &dummy_};
    [[no_unique_address]] Allocator alloc_;
};
