#include <iostream>
#include <list>
#include <unordered_map>

template <typename K, typename V>
class LinkedHashMap {
public:
    LinkedHashMap(std::size_t capcity) : table_(capcity) {}

    void put(K const& key, V const& val) {
        if (table_.contains(key)) {
            table_[key]->second = val;
            return;
        }
        list_.emplace_back(key, val);
        table_[key] = std::prev(list_.end());
    }
    V const& get(K const& key) {
        if (table_.contains(key)) {
            return table_[key]->second;
        }
        list_.emplace_back(key, V());
        table_[key] = std::prev(list_.end());
        return table_[key]->second;
    }
    void remove(K const& key) {
        if (table_.contains(key)) {
            auto node = table_[key];
            table_.erase(key);
            list_.erase(node);
        }
    }
    void for_each(auto func) {
        for (auto& node : list_) {
            func(node);
        }
    }

private:
    using Node = std::pair<K, V>;
    std::unordered_map<K, typename std::list<Node>::iterator> table_;
    std::list<Node> list_;
};

int main() {
    LinkedHashMap<int, int> map(10);
    for (int i = 0; i < 10; i++) {
        map.put(i, i);
    }
    map.remove(5);
    map.for_each(
        [&](auto& node) { std::cout << node.first << " " << node.second << " " << map.get(node.first) << '\n'; });
}
