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
#include "../utils/Metadata.hpp"
#include "./Zone.hpp"

class Run {
private:
    /* data */
    BF::BloomFilter bloomFilter;
    FencePointer fencePointer;
    std::string filePath;
    int level;
    int size;
    int num_zones;
    int num_elements_per_zone;
    std::vector<Zone> zones;

    bool isInBloomFilter(int key);
    bool isInFencePointer(int key);
public:
    Run(Metadata metadata, const std::map<int, Value>& map);
    Run(Metadata metadata);
    Value query(int key);
    std::map<int, Value> range_query(int min_key, int max_key);
    std::map<int, Value> readDisk(long start_pos, long end_pos); // read in block
    std::map<int, Value> readRun();
    const string &getFilePath() const;
    int getLevel() const;
    int getSize() const;
    void setLevel(int level);
    Metadata getInfo();

    int getMin() const;
    int getMax() const;
};

#endif /* LSM_TREE_SST_H */

