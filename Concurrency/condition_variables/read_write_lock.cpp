#include <mutex>
#include <semaphore>

using namespace std;

int readers = 0;
mutex read_mtx;
binary_semaphore write_sem{1};
binary_semaphore write_lock{0};

void readlock_acquire() {
    write_lock.acquire();
    unique_lock<mutex> lock(read_mtx);
    readers++;
    if (readers == 1)
        write_sem.acquire();
    write_lock.release();
}

void readlock_release() {
    unique_lock<mutex> lock(read_mtx);
    readers--;
    if (readers == 0)
        write_sem.release();
}

void writelock_acquire() {
    write_lock.acquire();
    write_sem.acquire();
}

void writelock_release() {
    write_lock.release();
    write_sem.release();
}



