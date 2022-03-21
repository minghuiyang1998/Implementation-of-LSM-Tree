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

std::vector<Value> Run::range_query(int min_key, int max_key) {

}

Run::Run() {
    // TODO: generate sst and store
    // TODO: call another constructor to setup the config

}

Run::Run() {

}
