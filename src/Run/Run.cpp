#include "Run.hpp"

bool Run::isInBloomFilter(int key) {
    return bloomFilter.query(to_string(key));
}

bool Run::isInFencePointer(int key) {
    return fencePointer.query(key);
}

Value Run::query(int key) {
    bool isInBF = isInBloomFilter(key);
    bool isInFP = isInFencePointer(key);

    if (!isInBF || !isInFP) return {};
    std::map<int, Value> map = readDisk();
    return map[key];
}

std::vector<Value> Run::range_query(int min_key, int max_key) {
    int fp_min = fencePointer.getMin();
    int fp_max = fencePointer.getMax();
    // not available element in this sst
    if (min_key > fp_max || max_key < fp_min) return {};

    std::vector<Value> results;
    std::map<int, Value> map = readDisk();
    for (const auto& element : map) {
        int key = element.first;
        if (key <= fp_max && key >= min_key) {
            Value val = element.second;
            results.push_back(val);
        }
    }
    return results;
}

Run::Run(int size, int level, std::string filePath, const std::map<int, Value>& map) {
    this->size = size;
    this->level = level;
    this->filePath = filePath;
    for (const auto& element : map) {
        std::string key = to_string(element.first);
        bloomFilter.program(key);
        fencePointer.program(element.first);
    }
}

std::map<int, Value> Run::readDisk() {
    // TODO: copy from db.cpp, use *file to get data

    return {};
}

const string &Run::getFilePath() const {
    return filePath;
}

int Run::getLevel() const {
    return level;
}

int Run::getSize() const {
    return size;
}



