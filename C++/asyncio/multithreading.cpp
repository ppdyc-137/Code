#include <thread>
#include <format>
#include <iostream>
#include <string>

void thread_timer(int sec) {
    while(true) {
        std::cout << std::format("sleep for {} seconds!\n", sec);
        std::this_thread::sleep_for(std::chrono::seconds(sec));
    }
}

void thread_echo() {
    std::string buf;
    while(true) {
        // std::cin >> buf;
        std::getline(std::cin, buf, '\n');
        std::cout << buf << '\n';
    }
}

int main() {
    std::thread t1(thread_timer, 10);
    std::thread t2(thread_echo);

    t1.join();
    t2.join();
}
