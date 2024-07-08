#include <iostream>
#include <thread>
#include <semaphore>
#include <chrono>
#include <mutex>

#include "buffer.h"

using namespace std;

mutex mtx;
Buffer buffer;

binary_semaphore sem_empty{MAX};
binary_semaphore sem_full{0};

void T_producer(int loops) {
    for (int i = 0; i < loops; ++i) {
        sem_empty.acquire();
        {
            unique_lock<mutex> lock(mtx);
            buffer.put(i);
        }
        sem_full.release();
    }
}

void T_consumer(int loops) {
    for (int i = 0; i < loops; ++i) {
        sem_full.acquire();
        {
            unique_lock<mutex> lock(mtx);
            int tmp = buffer.get();
        }
        sem_empty.release();
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
