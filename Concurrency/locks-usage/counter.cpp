#include "define.h"

#define N 4

using namespace std;

class Counter {
private:
    long value = 0;
    mutex mtx;

public:
    void increment() {
        unique_lock<mutex> lock(mtx);
        value++;
    }

    [[nodiscard]] long get() const {
        return value;
    }
};

Counter counter;

void count() {
    for (int i = 0; i < 1000000; i++) {
        counter.increment();
    }
}


int main() {
    thread threads[N];

    auto start = chrono::high_resolution_clock::now();

    for (auto &t: threads)
        t = thread(count);
    for (auto &t: threads)
        t.join();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Threads: " << N << endl;
    cout << "Counter value: " << counter.get() << endl;
    cout << "Time taken: " << duration << " ms" << endl;

    return 0;
}
