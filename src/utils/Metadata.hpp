#ifndef TEMPLATEDB_METADATA_HPP
#define TEMPLATEDB_METADATA_HPP

#include <vector>
#include "../Run/Zone.hpp"

class Metadata {
public:
    int bf_numElement;
    int bf_bitsPerElement;
    std::vector<bool> bf_vec;
    int fp_min;
    int fp_max;
    std::string filePath;
    int level;
    int size;
    int num_zones;
    const int num_elements_per_zone = 1000;
    std::vector<Zone> zones;

    Metadata(std::string filePath, int level, int size) {
        this->filePath = filePath;
        this->level = level;
        this->size = size;
    }
};

#endif //TEMPLATEDB_METADATA_HPP
