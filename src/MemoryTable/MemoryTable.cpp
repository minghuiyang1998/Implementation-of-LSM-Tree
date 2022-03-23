//
// Created by Shun Yao on 2022/3/20.
//
#include "MemoryTable.hpp"

map<int, Value> MemoryTable::clear() {
    this->map.clear();
    return this->map;
}

void MemoryTable::put(int key, Value value) {
    if (!value.visible)
        this->pointDelete(key, value);
    this->map.insert(pair<int, Value>(key, value));
}

Value MemoryTable::query(int key) {
    auto iter = this->map.find(key);
    if (iter != map.end()) {
        return iter->second;
    } else {
        return {}; // TODO: right ?
    }
}

// TODO: lower_bound error with cmake
//std::vector<Value> MemoryTable::range_query(int min_key, int max_key) {
//    auto low_position = std::lower_bound(map.begin(), map.end(), min_key);
//    auto high_position = std::upper_bound(map.begin(), map.end(), max_key);
//    // The range we need is  [low, high)
//    vector<Value> ret;
//    for(std::map<int, Value>::iterator iter = low_position; iter != high_position; iter++) {
//        ret.push_back(iter->second);
//    }
//    return ret;
//}

void MemoryTable::pointDelete(int key, Value value) {
    this->map.insert(pair<int, Value>(key, value));
}


