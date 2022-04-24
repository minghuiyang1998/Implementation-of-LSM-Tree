#ifndef TEMPLATEDB_METADATA_HPP
#define TEMPLATEDB_METADATA_HPP

#include <vector>
#include "../Run/Zone.hpp"

class Metadata {
private:
    int bf_numElement = 1024;
    int bf_bitsPerElement = 64;
    std::vector<bool> bf_vec;
    int fp_min = INT32_MAX;
    int fp_max = INT32_MIN;
    std::string filePath;
    int level;   // the number of first level is 0
    int size;   // the size of the run
    int num_zones;
    int num_elements_per_zone = 2;
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

    Metadata() {}

    Metadata(std::string filePath, int level, int size) {
        this->filePath = filePath;
        this->level = level;
        this->size = size;
    }

    Metadata(
            int bf_numElement,
            int bf_bitsPerElement,
            std::vector<bool> bf_vec,
            int fp_min,
            int fp_max,
            std::string filePath,
            int level,
            int size,
            int num_zones,
            int num_elements_per_zone,
            std::vector<Zone> zones
    ) {
        this->bf_numElement = bf_numElement;
        this->bf_bitsPerElement = bf_bitsPerElement;
        this->bf_vec = bf_vec;
        this->fp_max = fp_max;
        this->fp_min = fp_min;
        this->filePath = filePath;
        this->level = level;
        this->size = size;
        this->num_zones = num_zones;
        this->num_elements_per_zone = num_elements_per_zone;
        this->zones = zones;
    }

};

#endif //TEMPLATEDB_METADATA_HPP
