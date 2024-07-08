#pragma once

constexpr int MAX = 100;

class Buffer {
private:
    int fill = 0;
    int use = 0;
    int buffer[MAX]{};
public:
    int count = 0;

public:
    void put(int val) {
        buffer[fill] = val;
        fill = (fill + 1) % MAX;
        count ++;
    }

    int get() {
        int tmp = buffer[use];
        use = (use + 1) % MAX;
        count --;
        return tmp;
    }
};

