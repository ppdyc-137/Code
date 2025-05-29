#include <iostream>
#include <list>
#include <unordered_map>

class LFUCache {
public:
    LFUCache(int capacity) : capacity(capacity) {}

    int get(int key) {
        if (!keyNodeMap.contains(key)) {
            return -1;
        }

        auto iter = update(key);
        return iter->val;
    }

    void put(int key, int value) {
        if (keyNodeMap.contains(key)) {
            auto iter = update(key);
            iter->val = value;
            return;
        }

        if (keyNodeMap.size() == capacity) {
            auto& list = freqNodeList[minFreq];
            auto node = list.back();
            keyNodeMap.erase(node.key);
            list.pop_back();
            if (list.empty()) {
                freqNodeList.erase(minFreq);
            }
        }

        minFreq = 1;
        freqNodeList[1].push_front(Node{key, value, 1});
        auto iter = freqNodeList[1].begin();
        keyNodeMap[key] = iter;
    }

private:
    struct Node {
        int key;
        int val;
        int freq;
    };

    std::list<Node>::iterator update(int key) {
        auto iter = keyNodeMap[key];
        auto freq = iter->freq;
        auto& old_list = freqNodeList[freq];
        auto& new_list = freqNodeList[freq + 1];
        new_list.splice(new_list.begin(), old_list, iter);
        if (old_list.empty()) {
            freqNodeList.erase(freq);
            if (minFreq == freq) {
                minFreq++;
            }
        }
        iter->freq++;
        return iter;
    }

    std::unordered_map<int, std::list<Node>> freqNodeList;
    std::unordered_map<int, std::list<Node>::iterator> keyNodeMap;
    int capacity;
    int minFreq;
};

int main() {
    LFUCache lfu(2);
    lfu.put(1, 1);
    lfu.put(2, 2);
    std::cout << lfu.get(1) << '\n';
    lfu.put(3, 3);
    std::cout << lfu.get(2) << '\n';
    std::cout << lfu.get(3) << '\n';
    lfu.put(4, 4);
    std::cout << lfu.get(1) << '\n';
    std::cout << lfu.get(3) << '\n';
    std::cout << lfu.get(4) << '\n';
}
