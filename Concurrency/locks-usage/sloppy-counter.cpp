#include "define.h"

#define N 24
#define S 1024

using namespace std;

class Counter {
private:
    long value = 0;
    mutex mtx;
    Counter* globalCounter;

public:
    explicit Counter(Counter* c = nullptr) : globalCounter(c) {};

    void increment() {
        unique_lock<mutex> lock(mtx);
        value++;
        if (value == S && globalCounter) {
            globalCounter->incrementS();
            value = 0;
        }
    }

    void incrementS() {
        unique_lock<mutex> lock(mtx);
        value += S;
    }

    [[nodiscard]] long get() const {
        return value;
    }
};

Counter* globalCounter = new Counter();

void count(Counter* counter) {
    for (int i = 0; i < 1000000; i++) {
        counter->increment();
    }
}


int main() {
    thread threads[N];

    auto start = chrono::high_resolution_clock::now();

    for (auto &t: threads) {
        auto counter = new Counter(globalCounter);
        t = thread(count, counter);
    }
    for (auto &t: threads)
        t.join();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Threads: " << N << endl;
    cout << "Sloppiness: " << S << endl;
    cout << "Counter value: " << globalCounter->get() << endl;
    cout << "Time taken: " << duration << " ms" << endl;

    return 0;
}
