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
    int num_elements_per_zone;
    std::vector<Zone> zones;
};

#endif //TEMPLATEDB_METADATA_HPP
