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
#include "./Zone.hpp"

/**
 * A Run file is like
 * 1      // level
 * 3      // size
 * 2,true,1,3,5,6,7,8  // key, visible, timestamp, items
 * 4,true,2,2,4
 * 7,true,3,10
 */

class Run {
private:
    /* data */
    BF::BloomFilter bloomFilter;
    FencePointer fencePointer;
    std::string filePath;
    int level;
    int size;
    int num_zones;             // TODO: config
    int num_elements_per_zone; // TODO: config
    std::vector<Zone> zones;
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
    void setLevel(int level);
    void setFilePath(const string &filePath);
};

#endif /* LSM_TREE_SST_H */

