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
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
    public:
        SSTable(/* args */);
        int query(int key);
};
#endif /* LSM_TREE_SST_H */

