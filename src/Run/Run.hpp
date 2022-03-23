#ifndef LSM_TREE_SST_H
#define LSM_TREE_SST_H

#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <string>
#include <sstream>
#include <map>

#include "FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "../utils/Value.hpp"



class Run {
    private:
        /* data */
        BF::BloomFilter bloomFilter;
        FencePointer fencePointer;
        std::string filePath;
        int level;
        int size;
        bool isInBloomFilter(int key);
        bool isInFencePointer(int key);
        bool parsebool(std::string str);
    public:
        Run(int size, int level, std::string filePath, const std::map<int, Value>& map);
        Value query(int key);
        std::vector<Value> range_query(int min_key, int max_key);
        std::map<int, Value> readDisk();
        const string &getFilePath() const;
        int getLevel() const;
        int getSize() const;
};

#endif /* LSM_TREE_SST_H */

