#include "define.h"

class Queue {
    struct node {
        int val{};
        node* next = nullptr;

        node() = default;
        explicit node(int val) : val(val) {};
    };

private:
    node* head;
    node* tail;
    mutex head_mtx;
    mutex tail_mtx;

public:
    Queue() {
        head = tail = new node();
    }

    void enqueue(int val) {
        node* tmp = new node(val);
        unique_lock<mutex> lock(tail_mtx);
        tail->next = tmp;
        tail = tmp;
    }

    void dequeue() {
        unique_lock<mutex> lock(head_mtx);
        node* tmp = head;
        node* new_head = tmp->next;
        if (!new_head) {
            return;
        }
        head = new_head;
        delete tmp;
    }

    void print() {
        cout << "queue: ";
        node* tmp = head;
        while(tmp->next) {
            tmp = tmp->next;
            cout << tmp->val << " ";
        }
        cout << "\n";
    }
};

int main() {
    Queue q;
    for (int i = 0; i < 10; i ++) {
        q.enqueue(i);
    }
    for (int i = 0; i < 11; i ++) {
        q.dequeue();
        q.print();
    }
}
