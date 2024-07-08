#include "define.h"

class List {
    struct node {
        int value{};
        node* next = nullptr;

        node() = default;
        explicit node(int val) : value(val) {};
    };

private:
    node* root = nullptr;
    mutex mtx;

public:
    void push_front(int val) {
        node* n = new node(val);
        unique_lock<mutex> lock(mtx);
        n->next = root;
        root = n;
    }
};
