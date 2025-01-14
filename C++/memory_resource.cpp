#include <chrono>
#include <list>
#include <memory_resource>
#include <iostream>
#include <vector>
#include <format>

struct memory_resource_inspector : std::pmr::memory_resource {
public:
    explicit memory_resource_inspector(std::pmr::memory_resource* upstream) : _M_upstream(upstream) {}

private:

    void* do_allocate(size_t bytes, size_t alignment) override {
        void* p = _M_upstream->allocate(bytes, alignment);
        std::cout << std::format("allocate {} {} {}", p, bytes, alignment) << "\n";
        return p;
    }

    void do_deallocate(void* p, size_t bytes, size_t alignment) override {
        _M_upstream->deallocate(p, bytes, alignment);
        std::cout << std::format("deallocate {} {} {}", p, bytes, alignment) << "\n";
    }

    bool do_is_equal(const memory_resource&other) const noexcept override {
        return _M_upstream->is_equal(other);
    }

    std::pmr::memory_resource* _M_upstream;
};

int main() {
    {
        std::pmr::monotonic_buffer_resource m;
        memory_resource_inspector mem{&m};

        std::pmr::vector<int> v{&mem};
        for (int i = 0; i < 4096; i++) {
            v.emplace_back(i);
        }
    }

    // {
    //     std::list<char> list;
    //     auto start = std::chrono::high_resolution_clock::now();
    //     for (int i = 0; i < 65535; i++) {
    //         list.emplace_back('a');
    //     }
    //     auto end = std::chrono::high_resolution_clock::now();
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    //     std::cout << "list: " << duration << "us\n";
    // }
    // {
    //     std::pmr::monotonic_buffer_resource mem{65535*24};
    //     std::pmr::list<char> list{&mem};
    //     auto start = std::chrono::high_resolution_clock::now();
    //     for (int i = 0; i < 65535; i++) {
    //         list.emplace_back('a');
    //     }
    //     auto end = std::chrono::high_resolution_clock::now();
    //     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    //     std::cout << "pmr_list: " << duration << "us\n";
    // }
}
