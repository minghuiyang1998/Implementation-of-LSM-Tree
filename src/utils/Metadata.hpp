#ifndef TEMPLATEDB_METADATA_HPP
#define TEMPLATEDB_METADATA_HPP

#include <vector>
#include "../Run/Zone.hpp"

class Metadata {
private:
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
public:
    int getBfNumElement() const {
        return bf_numElement;
    }

    int getBfBitsPerElement() const {
        return bf_bitsPerElement;
    }

    const vector<bool> &getBfVec() const {
        return bf_vec;
    }

    int getFpMin() const {
        return fp_min;
    }

    int getFpMax() const {
        return fp_max;
    }

    const string &getFilePath() const {
        return filePath;
    }

    int getLevel() const {
        return level;
    }

    int getSize() const {
        return size;
    }

    int getNumZones() const {
        return num_zones;
    }

    int getNumElementsPerZone() const {
        return num_elements_per_zone;
    }

    const vector<Zone> &getZones() const {
        return zones;
    }

    void setBfNumElement(int bfNumElement) {
        bf_numElement = bfNumElement;
    }

    void setBfBitsPerElement(int bfBitsPerElement) {
        bf_bitsPerElement = bfBitsPerElement;
    }

    void setBfVec(const vector<bool> &bfVec) {
        bf_vec = bfVec;
    }

    void setFpMin(int fpMin) {
        fp_min = fpMin;
    }

    void setFpMax(int fpMax) {
        fp_max = fpMax;
    }

    void setFilePath(const string &filePath) {
        Metadata::filePath = filePath;
    }

    void setLevel(int level) {
        Metadata::level = level;
    }

    void setSize(int size) {
        Metadata::size = size;
    }

    void setNumZones(int numZones) {
        num_zones = numZones;
    }

    void setNumElementsPerZone(int numElementsPerZone) {
        num_elements_per_zone = numElementsPerZone;
    }

    void setZones(const vector<Zone> &zones) {
        Metadata::zones = zones;
    }
};

#endif //TEMPLATEDB_METADATA_HPP
