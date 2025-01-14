#include <atomic>

struct Counter {
    static constexpr uint64_t is_zero = 1ULL << 63;
    static constexpr uint64_t helped = 1ULL << 62;
    std::atomic_uint64_t counter{1};

    bool increment_if_not_zero() {
        return (counter.fetch_add(1) & is_zero) == 0;
    }

    bool decrement() {
        if( counter.fetch_sub(1) == 1 ) {
            uint64_t current = 0;
            if (counter.compare_exchange_strong(current, is_zero))
                return true;
            else if ((current & helped) && counter.exchange(is_zero) & helped)
                return true;
        }
        return false;
    }

    uint64_t read() {
        auto current = counter.load();
        if (current == 0) {
            if (counter.compare_exchange_strong(current, is_zero | helped))
                return 0;
        }
        return (current & is_zero) ? 0 : current;
    }

};

int main() {
}
