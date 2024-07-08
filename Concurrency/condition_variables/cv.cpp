#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "buffer.h"

using namespace std;

mutex mtx;
condition_variable cond_empty;
condition_variable cond_full;

Buffer buffer;

void T_producer(int loops) {
    for (int i = 0; i < loops; ++i) {
        unique_lock<mutex> lock(mtx);
        while (buffer.count == MAX)
            cond_empty.wait(lock);
        buffer.put(i);
        cond_full.notify_one();
    }
}

void T_consumer(int loops) {
    for (int i = 0; i < loops; ++i) {
        int tmp;
        {
            unique_lock<mutex> lock(mtx);
            while(buffer.count == 0)
                cond_full.wait(lock);
            tmp = buffer.get();
            cond_empty.notify_one();
        }
//        cout << tmp << "\n";
    }
}


int main() {
    thread producer_threads[10];
    thread consumer_threads[10];

    auto start = chrono::high_resolution_clock::now();
    for (auto& t: producer_threads)
        t = thread(T_producer, 10000);
    for (auto& t: consumer_threads)
        t = thread(T_consumer, 10000);
    for (auto& t: producer_threads)
        t.join();
    for (auto& t: consumer_threads)
        t.join();
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Time taken: " << duration << " ms" << endl;
    return 0;
}
