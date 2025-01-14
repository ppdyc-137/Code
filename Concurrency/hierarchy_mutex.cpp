#include <climits>
#include <mutex>
#include <stdexcept>
#include <thread>

class hierarchical_mutex {
  private:
    std::mutex internal_mutex;
    static thread_local unsigned long this_thread_hierarchy_value;

    unsigned long hierarchy_value;
    unsigned long previous_hierarchy_value;

    void check_for_hierarchy_violation() {
        if (hierarchy_value > this_thread_hierarchy_value) {
            throw std::logic_error("mutex hierarchy voilated");
        }
    }

    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }

  public:
    hierarchical_mutex(unsigned long value) : hierarchy_value(value) {}
    void lock() {
        check_for_hierarchy_violation();
        internal_mutex.lock();
        update_hierarchy_value();
    }
    void unlock() {
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }
    bool try_lock() {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock()) {
            return false;
        }
        update_hierarchy_value();
        return true;
    }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULONG_MAX);

hierarchical_mutex high_level_mutex(1000);
hierarchical_mutex lowel_level_mutex(500);
hierarchical_mutex other_mutex(50);

void lowel_level_func() {
    std::lock_guard lock(lowel_level_mutex);
}

void high_level_func() {
    std::lock_guard lock(high_level_mutex);
    lowel_level_func();
}

void other_func() {
    std::lock_guard lock(other_mutex);
    high_level_func();
}

int main() {
    std::thread t{other_func};
    t.join();
}
