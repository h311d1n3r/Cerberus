#ifndef CERBERUS_VALUE_ORDERED_MAP_H
#define CERBERUS_VALUE_ORDERED_MAP_H

#include <vector>
#include <string>

template <typename KeyType, typename ValueType>
class value_ordered_map {
private:
    std::vector<std::pair<KeyType, ValueType>> map;
    int64_t quick_sort_partition(int64_t low, int64_t high) {
        ValueType pivot = this->map[high].second;
        int64_t i = (low - 1);
        if(i < 0) i = 0;
        for(int64_t j = low; j <= high - 1; j++) {
            if((invert_sorting && this->map[j].second > pivot) || (!invert_sorting && this->map[j].second < pivot)) {
                std::swap(this->map[i], this->map[j]);
            }
        }
        std::swap(this->map[i+1], this->map[high]);
        return i+1;
    }
    void quick_sort(int64_t low, int64_t high) {
        if (low < high) {
            size_t part = quick_sort_partition(low, high);
            quick_sort(low, part - 1);
            quick_sort(part + 1, high);
        }
    }
    bool invert_sorting = false;
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
        invert_sorting = false;
        quick_sort(0, this->map.size()-1);
    }
    void invert_sort() {
        invert_sorting = true;
        quick_sort(0, this->map.size()-1);
    }
    std::pair<KeyType, ValueType> at(int64_t i) {
        return this->map[i];
    }
    size_t size() {
        return this->map.size();
    }
};

#endif //CERBERUS_VALUE_ORDERED_MAP_H
