#ifndef LSM_TREE_SST_H
#define LSM_TREE_SST_H

#include "FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "../SSTable/TableReader.hpp"
#include "string"

class Run {
    private:
        /* data */
        BF::BloomFilter bloomFilter;
        FencePointer fencePointer;
        TableReader fp;
        string id;
        int size;
        int level;
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
    public:
        Run(unordered_map<int, Value> map);
        Run();
        Value query(int key);
        std::vector<Value> range_query(int min_key, int max_key);
};
#endif /* LSM_TREE_SST_H */

