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

    // TODO: read Data find the value
}

