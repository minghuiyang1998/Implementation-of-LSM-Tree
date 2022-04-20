//
// Created by Yang Minghui on 2022/4/20.
//

#ifndef TEMPLATEDB_METADATA_HPP
#define TEMPLATEDB_METADATA_HPP

class Metadata {
public:
    BF::BloomFilter bloomFilter;
    FencePointer fencePointer;
    std::string filePath;
    int level;
    int size;
    int num_zones;             // TODO: config
    int num_elements_per_zone; // TODO: config
    std::vector<Zone> zones;
};

#endif //TEMPLATEDB_METADATA_HPP
