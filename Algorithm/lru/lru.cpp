#include <iostream>
#include <list>
#include <unordered_map>

class LRU {
public:
    explicit LRU(int cap) : cap_(cap) {}

    int get(int key) {
        if (!map_.contains(key)) {
            return -1;
        }

        auto iter = map_[key];
        list_.splice(list_.begin(), list_, iter);
        return iter->value;
    }

    void set(int key, int val) {
        if (map_.contains(key)) {
            auto iter = map_[key];
            list_.splice(list_.begin(), list_, iter);
            iter->value = val;
            return;
        }

        if (list_.size() >= cap_) {
            auto node = list_.back();
            list_.pop_back();
            map_.erase(node.key);
        }
        auto node = Node{key, val};
        list_.push_front(node);
        map_[node.key] = list_.begin();
    }

private:
    struct Node {
        int key;
        int value;
    };
    int cap_;
    std::list<Node> list_;
    std::unordered_map<int, std::list<Node>::iterator> map_;
};

int main() {
    {
        LRU lru(2);
        lru.set(1, 11);
        lru.set(2, 22);
        // std::cout << lru.get(1) << '\n';
        lru.set(3, 33);
        std::cout << lru.get(1) << '\n';
    }
}
