#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename K, typename V>
class ArrayHashMap {
public:
    ArrayHashMap(std::size_t capcity) : table_(capcity) {}

    void put(K const& key, V const& val) {
        if (table_.contains(key)) {
            values_[table_[key]].second = val;
            return;
        }
        values_.emplace_back(key, val);
        table_[key] = values_.size() - 1;
    }
    V const& get(K const& key) {
        if (table_.contains(key)) {
            return values_[table_[key]].second;
        }
        values_.emplace_back(key, V());
        table_[key] = values_.size() - 1;
        return values_.back().second;
    }
    void remove(K const& key) {
        if (!table_.contains(key)) {
            return;
        }
        auto index = table_[key];
        table_[values_.back().first] = index;
        std::swap(values_[index], values_.back());
        table_.erase(key);
        values_.pop_back();
    }

private:
    using node = std::pair<K, V>;
    std::unordered_map<K, std::size_t> table_;
    std::vector<node> values_;
};

int main() {
    ArrayHashMap<int, int> map(10);
    for (int i = 0; i < 10; i++) {
        map.put(i, i);
    }
    map.remove(5);
    for (int i = 0; i < 10; i++) {
        std::cout << map.get(i) << '\n';
    }
}
