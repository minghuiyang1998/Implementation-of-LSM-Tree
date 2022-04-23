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
#include "../utils/Metadata.hpp"

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
    Run(Metadata metadata, const std::map<int, Value>& map);
    Run(Metadata metadata);
    Metadata getInfo();
    Value query(int key);
    std::vector<Value> range_query(int min_key, int max_key);
    std::map<int, Value> readDisk(int start_pos, int end_pos);
    const string &getFilePath() const;
    int getLevel() const;
    int getSize() const;
    void setLevel(int level);
    void setFilePath(const string &filePath);
};

#endif /* LSM_TREE_SST_H */

