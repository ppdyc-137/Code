#include <deque>
#include <vector>
using namespace std;

template<typename T>
class MonotonicQueue {
public:
    void push(T const& value) {
        q_.push_back(value);

        while (!max_q_.empty() && max_q_.back() < value) {
            max_q_.pop_back();
        }
        max_q_.push_back(value);

        while (!min_q_.empty() && min_q_.back() > value) {
            min_q_.pop_back();
        }
        min_q_.push_back(value);
    }
    void pop() {
        auto res = q_.front();
        q_.pop_front();

        if (!max_q_.empty() && res == max_q_.front()) {
            max_q_.pop_front();
        }
        if (!min_q_.empty() && res == min_q_.front()) {
            min_q_.pop_front();
        }
    }

    T const & max() {
        return max_q_.front();
    }
    T const & min() {
        return min_q_.front();
    }

private:
    std::deque<T> q_;
    std::deque<T> min_q_;
    std::deque<T> max_q_;
};

int main() {
    auto v = std::vector{1, 2, 1};
    auto q = MonotonicQueue<int>();
    for (auto i : v) {
        q.push(i);
    }
}
