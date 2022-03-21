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
        std::fstream *file;
        string id;
        int size;
        int level;
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
        std::map<int, Value> readDisk();
    public:
        Run(string id, int level, std::fstream *file, const std::map<int, Value>& map);
        Value query(int key);
        std::vector<Value> range_query(int min_key, int max_key);
        void closeFile();
};
#endif /* LSM_TREE_SST_H */

