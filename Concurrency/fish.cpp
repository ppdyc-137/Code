#include <condition_variable>
#include <cstdio>
#include <thread>

enum { A = 1, B, C, D, E, F };

struct rule {
    int from;
    char ch;
    int to;
} rules[] = {
    { A, '<', B},
    { B, '>', C},
    { C, '<', D},
    { D, ' ', A},
    { A, '>', E},
    { E, '<', F},
    { F, '>', D},
};

int current = A;

int next(char ch) {
    for (const auto& rule : rules) {
        if (rule.from == current && rule.ch == ch) {
            return rule.to;
        }
    }
    return 0;
}

bool can_print(char ch) {
    return next(ch) != 0;
}

std::mutex m;
std::condition_variable cond;
void fish_thread(char ch) {
    while(true) {
        std::unique_lock lk(m);
        cond.wait(lk, [ch]() { return can_print(ch); });

        putchar(ch);
        current = next(ch);

        cond.notify_all();
    }
}

int main() {
    std::thread t1(fish_thread, '<');
    std::thread t2(fish_thread, '>');
    std::thread t3(fish_thread, ' ');
    t1.join();
    t2.join();
    t3.join();
}
