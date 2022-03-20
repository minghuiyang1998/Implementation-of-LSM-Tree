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
        Run(/* args */);
        Value query(int key);
};
#endif /* LSM_TREE_SST_H */

