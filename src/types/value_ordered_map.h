#ifndef CERBERUS_VALUE_ORDERED_MAP_H
#define CERBERUS_VALUE_ORDERED_MAP_H

#include <vector>
#include <string>
#include <algorithm>

template <typename KeyType, typename ValueType>
class value_ordered_map {
private:
    std::vector<std::pair<KeyType, ValueType>> map;
public:
    ValueType& operator[](const KeyType key) {
        for(std::pair<KeyType, ValueType>& pair : this->map) {
            if(pair.first == key) return pair.second;
        }
        std::pair<KeyType, ValueType> new_pair = std::pair<KeyType, ValueType>(key, 0);
        this->map.push_back(new_pair);
        return this->map.at(this->map.size()-1).second;
    }
    void sort() {
        std::sort(map.begin(), map.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
    }
    void invert_sort() {
        std::sort(map.begin(), map.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
    }
    std::pair<KeyType, ValueType> at(int64_t i) {
        return this->map[i];
    }
    size_t size() {
        return this->map.size();
    }
};

#endif //CERBERUS_VALUE_ORDERED_MAP_H
