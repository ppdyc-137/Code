#include <memory>
#include <optional>
#include <sys/mman.h>
#include <iostream>
#include <cstdio>

#define GiB * (1024LL * 1024 * 1024)

using namespace std;

template <typename T>
struct MmapDeleter {
    size_t length;

    explicit MmapDeleter(size_t length) : length(length) {};

    void operator()(T *ptr) const {
        cout << "munmap\n";
        munmap(ptr, length);
    }
};

template <typename T>
optional<unique_ptr<T, MmapDeleter<T>>> my_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    T *ptr = static_cast<T*>(mmap(addr, length, prot, flags, fd, offset));
    if (ptr == MAP_FAILED) {
        return nullopt;
    }
    return unique_ptr<T, MmapDeleter<T>>(ptr, MmapDeleter<T>(length));
}

int main() {
    auto ptr = my_mmap<uint8_t>(NULL, 8 GiB, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (!ptr.has_value()) {
        perror("mmap failed");
        exit(1);
    }

    printf("mmap: %lx\n", (uintptr_t) ptr.value().get());
    *(ptr.value().get() + 2 GiB) = 2;
    *(ptr.value().get() + 3 GiB) = 3;
    *(ptr.value().get() + 7 GiB) = 7;

    printf("%d\n", *(ptr.value().get() + 2 GiB));
    printf("%d\n", *(ptr.value().get() + 3 GiB));
    printf("%d\n", *(ptr.value().get() + 7 GiB));
}
