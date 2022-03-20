#ifndef LSM_TREE_SST_H
#define LSM_TREE_SST_H

#include "FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "../SSTable/TableManager.hpp"
#include "string"

class Run {
    private:
        /* data */
        BF::BloomFilter bloomFilter;
        FencePointer fencePointer;
        TableManager fp;
        string id;
        int size;
        int level;
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
    public:
        Run(/* args */);
        int query(int key);
};
#endif /* LSM_TREE_SST_H */

