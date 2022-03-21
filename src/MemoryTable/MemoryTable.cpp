//
// Created by Shun Yao on 2022/3/20.
//
#include "MemoryTable.hpp"


map<int, Value> MemoryTable::getMap() {}() {

   return this->map;
}

void MemoryTable::clear() {
    this->map.clear();
}


void MemoryTable::put(int key, Value value) {
    if (value.visible == false)
        this->pointDelete(key, value);
    this->map.insert(pair<int, Value>(key, value));
}

Value MemoryTable::query(int key) {
    auto iter = this->map.find(key);

    if (iter != map.end())
        return iter->second;
    else
        return nullptr; //需要商量一下没找到的话要返回什么
}

vector<Value> MemoryTable::rangeQuery(int key_low, int key_high) {

}

void MemoryTable::pointDelete(int Key, Value value) {
    this->map.insert(pair<int, Value>(key, value));
}

