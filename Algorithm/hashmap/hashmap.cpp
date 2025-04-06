#include <cstddef>
#include <functional>
#include <iostream>
#include <list>
#include <utility>
#include <vector>

template <typename K, typename V>
class HashMap {
public:
    HashMap(std::size_t capcity) : table_(capcity) {}

    void put(K const& key, V const& val) {
        auto& list = table_[hash(key)];
        for (auto& pair : list) {
            if (pair.first == key) {
                pair.second = val;
                return;
            }
        }
        list.emplace_back(key, val);
        size_++;

        auto cap = table_.size();
        if (size_ >= (cap - cap / 4)) {
            resize(table_.size() * 2);
        }
    }
    V const& get(K const& key) {
        auto& list = table_[hash(key)];
        for (auto& pair : list) {
            if (pair.first == key) {
                return pair.second;
            }
        }
        list.emplace_back(key, V());
        return list.back().second;
    }
    void remove(K const& key) {
        auto& list = table_[hash(key)];
        for (auto it = list.begin(); it != list.end(); it++) {
            if (it->first == key) {
                list.erase(it);
                size_--;

                if (size_ <= table_.size() / 8) {
                    resize(table_.size() / 4);
                }
                return;
            }
        }
    }

private:
    std::size_t hash(V const& val) { return std::hash<V>{}(val) % table_.size(); }

    void resize(std::size_t cap) {
        std::cout << "resize from " << table_.size() << " to " << cap << "\n";
        HashMap newMap(cap);
        for (auto const& list : table_) {
            for (auto const& pair : list) {
                newMap.put(pair.first, pair.second);
            }
        }
        table_ = newMap.table_;
    }

    std::size_t size_{};
    std::vector<std::list<std::pair<K, V>>> table_;
};

int main() {
    HashMap<int, int> map(10);
    for (int i = 0; i < 10; i++) {
        map.put(i, i);
    }
    map.remove(5);
    for (int i = 0; i < 10; i++) {
        std::cout << map.get(i) << '\n';
    }
}
