//
// Created by Shun Yao on 2022/3/20.
//
#include "MemoryTable.hpp"

std::map<int, Value> MemoryTable::clear() {
    std::map<int, Value> res;
    res.insert(this->memoMap.begin(), this->memoMap.end());
    this->memoMap.clear();
    return res;
}

std::map<int, Value> & MemoryTable::getMap() {
    return memoMap;
}

void MemoryTable::put(int key, Value value) {
    // insert will not override old value will be override by new value in memoMap
    // this->memoMap.insert(pair<int, Value>(key, value));
    this->memoMap[key] = value;
}

Value MemoryTable::query(int key) {
    bool isKey = this->memoMap.count(key) == 1;
    if (isKey) {
        return this->memoMap[key];
    } else {
        return Value(false);
    }
}

std::map<int, Value> MemoryTable::range_query(int min_key, int max_key) {
    auto low_position = memoMap.lower_bound(min_key);
    auto high_position = memoMap.upper_bound(max_key);
    // The range we need is  [low, high]
    std::map<int, Value> ret;
    for(auto iter = low_position; iter != high_position; iter++) {
        int key = iter->first;
        Value val = iter->second;
        ret[key] = val;
    }
    return ret;
}


