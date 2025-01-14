#include <cassert>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

class ThreadPool {
private:
    bool exit = false;
    std::mutex m;
    std::condition_variable cond;

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;

    void thread_run() {
        while(true) {
            std::function<void()> task;
            {
                std::unique_lock lk(m);
                cond.wait(lk, [this]() { return !tasks.empty() || exit; });
                if (exit && tasks.empty()) return;
                task = std::move(tasks.front());
                tasks.pop();
            }
            try {
                task();
            } catch (const std::exception& e) {
                std::fprintf(stderr, "task throwed an error: %s\n", e.what());
            }
        }
    }

public:
    explicit ThreadPool(int num) {
        threads.reserve(num);
        for (int i = 0; i < num; i++) {
            threads.emplace_back(&ThreadPool::thread_run, this);
        }
    }
    ~ThreadPool() {
        {
            std::lock_guard lk(m);
            exit = true;
        }
        cond.notify_all();
        for (auto& t: threads) {
            t.join();
        }
    }

    template<typename Fn, typename...Args>
    auto run(Fn&& f, Args&& ...args) {
        using ReturnType = std::invoke_result_t<Fn, Args...>;
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...));
        auto future = task->get_future();
        {
            std::lock_guard lk(m);
            tasks.push([task](){ (*task)(); });
        }
        cond.notify_all();
        return future;
    }
};

template<typename Fn, typename ...Args>
auto spawn_task(Fn&& f, Args&&... args) {
    using result_type = std::invoke_result_t<Fn, Args...>;

    // std::packaged_task<result_type(Args...)> task(std::move(f));
    // auto res = task.get_future();
    // std::thread t(std::move(task), std::move(std::forward<Args>(args)...));

    std::packaged_task<result_type()> task(std::bind(std::forward<Fn>(f), std::forward<Args>(args)...));
    auto res = task.get_future();
    std::thread t(std::move(task));

    t.detach();
    return res;
}

int do_something(int n) {
    for (int i = 0; i < n; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::printf("sleep for %d/%d\n", i+1, n);
    }
    return n;
}

int main() {
    ThreadPool pool(4);
    std::unordered_map<int, std::future<int>> results;
    for (int i = 1; i <= 4; i++) {
        auto res = pool.run(do_something, i);
        results.insert({i, std::move(res)});
    }
    for (auto& [i, res] : results) {
        assert(i == res.get());
    }
}
