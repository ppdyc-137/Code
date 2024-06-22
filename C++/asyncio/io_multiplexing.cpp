#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

struct GlobalData {
    GlobalData() {
        epfd = epoll_create(1);
        if (epfd < 0) {
            perror("epoll_create");
            exit(EXIT_FAILURE);
        }

        timerfd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (timerfd < 0) {
            perror("timerfd_create");
            exit(EXIT_FAILURE);
        }
    }

    ~GlobalData() {
        close(epfd);
        close(timerfd);
    }

    int epfd;
    int timerfd;
} g;

void echo() {
    char buf[1024]{};
    read(STDIN_FILENO, buf, 1024);
    printf("%s", buf);
}

void register_echo() {
    epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = (void *)echo;
    if (epoll_ctl(g.epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

void timer_handler() {
    printf("dd\n");
    uint64_t buf = 0;
    read(g.timerfd, &buf, sizeof(buf));
}

void register_timer(int sec) {
    itimerspec ts{
        .it_interval = timespec { .tv_sec = sec, .tv_nsec = 0},
        .it_value = timespec { .tv_sec = sec, .tv_nsec = 0},
    };
    if (timerfd_settime(g.timerfd, 0, &ts, NULL) < 0) {
        perror("timerfd_settime");
        exit(EXIT_FAILURE);
    }

    epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = (void *)timer_handler;
    if (epoll_ctl(g.epfd, EPOLL_CTL_ADD, g.timerfd, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }
}

int main() {
    constexpr int max_events = 2;
    struct epoll_event events[max_events];

    register_echo();
    register_timer(200);

    while(true) {
        int nfds = epoll_wait(g.epfd, events, max_events, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nfds; i++) {
             auto handle  = (void(*)())events[i].data.ptr;
             handle();
        }
    }
}
