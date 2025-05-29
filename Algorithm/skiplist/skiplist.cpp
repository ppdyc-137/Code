#include <algorithm>
#include <cstdlib>
#include <iostream>

#include <vector>
struct Node {
    Node(int key, int value, int level) : key(key), value(value), level(level) {
        forward = std::vector<Node*>(level+1);
    }

    int key;
    int value;

    int level;
    std::vector<Node*> forward;
};

class SkipList {
public:
    SkipList() {
        header = new Node(0, 0, max_level);
    }
    void set(int key, int value) {
        Node* cur = header;
        Node* update[max_level + 1]{};

        for (int i = cur_level; i >= 0; i--) {
            while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
                cur = cur->forward[i];
            }
            update[i] = cur;
        }

        cur = cur->forward[0];
        if (cur != nullptr && cur->key == key) {
            return;
        }

        int random_level = generate_random_level();
        if (random_level > cur_level) {
            for (int i = cur_level + 1; i <= random_level; i++) {
                update[i] = header;
            }
            cur_level = random_level;
        }

        Node* node = new Node(key, value, random_level);
        node->level = random_level;

        for (int i = 0; i <= random_level; i++) {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
        element_count++;
    }

    int get(int key) {
        Node* cur = header;
        for (int i = cur_level; i >= 0; i--) {
            while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
                cur = cur->forward[i];
            }
        }
        cur = cur->forward[0];
        if (cur != nullptr && cur->key == key) {
            return cur->value;
        }
        return -1;
    }

    void del(int key) {
        Node* cur = header;
        Node* update[max_level + 1]{};
        for (int i = cur_level; i >= 0; i--) {
            while (cur->forward[i] != nullptr && cur->forward[i]->key < key) {
                cur = cur->forward[i];
            }
        }
        if (cur->forward[0] == nullptr || cur->forward[0]->key != key) {
            return;
        }

        Node* del = cur->forward[0];

        for (int i = 0; i <= del->level; i++) {
            if (update[i]->forward[i] != del) {
                break;
            }
            update[i]->forward[i] = del->forward[i];
        }
        while (cur_level > 0 && header->forward[cur_level] == nullptr) {
            cur_level--;
        }

        delete del;
        element_count--;
    }

    int generate_random_level() {
        int level = 0;
        while (rand() % 4 == 0) {
            level++;
        }
        return std::min(level, max_level);
    }
    void display() {
        for(int i = cur_level; i >= 0; i--) {
            std::cout << "Level " << i << ": ";
            Node* node = header->forward[i];
            while (node != nullptr) {
                std::cout << node->key << " ";
                node = node->forward[i];
            }
            std::cout << "\n";
        }
    }

private:
    static constexpr int max_level = 10;
    int cur_level{};
    Node* header{};
    int element_count{};
};

int main() {
    SkipList sl{};
    for (int i = 0; i < 100; i++) {
        sl.set(i, i);
    }
    sl.display();
    std::cout << sl.get(50) << '\n';
}
