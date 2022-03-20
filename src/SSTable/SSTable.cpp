#include "SSTable.hpp"

SSTable::SSTable(/* args */)
{
    // 1. create file
    // 2.
}

bool SSTable::isInBloomFilter(int key) {
    return bloomFilter.query(to_string(key));
}

bool SSTable::isInFencePointer(int key) {
    return fencePointer.query(key);
}

int SSTable::query(int key) {

}