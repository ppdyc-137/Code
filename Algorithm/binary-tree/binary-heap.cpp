#include <cstddef>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>

// 核心原理是在插入和推出节点时，保持父节点与左右节点的大小关系
template <typename T>
class BinaryHeap {
    std::vector<T> heap_;
    std::size_t size_{1};
    using comparator = std::function<bool(T const&, T const&)>;
    comparator cmp_;

    static std::size_t parent(std::size_t node) { return node / 2; }
    static std::size_t left(std::size_t node) { return node * 2; }
    static std::size_t right(std::size_t node) { return node * 2 + 1; }

    void swap(std::size_t i, std::size_t j) { std::swap(heap_[i], heap_[j]); }

    void swim(std::size_t node) {
        while (node > 1 && cmp_(heap_[node], heap_[parent(node)])) {
            swap(parent(node), node);
            node = parent(node);
        }
    }

    void sink(std::size_t node) {
        while (left(node) < size_ || right(node) < size_) {
            auto min = node;
            if (left(node) < size_ && cmp_(heap_[left(node)], heap_[min])) {
                min = left(node);
            }
            if (right(node) < size_ && cmp_(heap_[right(node)], heap_[min])) {
                min = right(node);
            }
            if (min == node) {
                return;
            }

            swap(min, node);
            node = min;
        }
    }

public:
    BinaryHeap(std::size_t cap, comparator cmp) : cmp_(std::move(cmp)) { heap_.resize(cap); }

    int peak() { return heap_[1]; }

    void push(int val) {
        if (size_ == heap_.size()) {
            heap_.resize(size_ * 2);
        }
        heap_[size_] = val;
        swim(size_);
        size_++;
    }

    int pop() {
        auto top = heap_[1];
        heap_[1] = heap_[size_ - 1];
        size_--;
        sink(1);
        if (size_ < heap_.size() / 4) {
            heap_.resize(heap_.size() / 2);
        }
        return top;
    }

    std::size_t size() { return size_ - 1; }

    bool empty() { return size_ == 1; }
};

int main() {
    BinaryHeap<int> heap(10, [](auto& a, auto& b) { return a > b; });
    heap.push(4);
    heap.push(5);
    heap.push(2);
    heap.push(1);
    heap.push(9);
    heap.push(0);
    heap.push(4);
    heap.push(5);
    heap.push(2);
    heap.push(1);
    heap.push(9);
    heap.push(0);

    while (!heap.empty()) {
        std::cout << heap.pop() << '\n';
    }
}
