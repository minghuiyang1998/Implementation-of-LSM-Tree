#ifndef LSM_TREE_SST_H
#define LSM_TREE_SST_H

#include "FencePointer/FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "string"

class SSTable {
    private:
        /* data */
        BF::BloomFilter bloomFilter;
        FencePointer fencePointer;
        FILE *fp;
        string id;
        int size;
        int level;
        bool isInBloomFilter(string key);
        bool isInFencePointer(string key);
    public:
        SSTable(/* args */);
        int query(string key);
};
#endif /* LSM_TREE_SST_H */

