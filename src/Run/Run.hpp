#ifndef LSM_TREE_SST_H
#define LSM_TREE_SST_H

#include "FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "../utils/Value.hpp"
#include <map>
#include "string"

class Run {
    private:
        /* data */
        BF::BloomFilter bloomFilter;
        FencePointer fencePointer;
        std::string filePath;
        int level;
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
    public:
        Run(int size, int level, std::string filePath, const std::map<int, Value>& map);
        Value query(int key);
        std::vector<Value> range_query(int min_key, int max_key);
        std::map<int, Value> readDisk();
        int size;
};
#endif /* LSM_TREE_SST_H */

