#ifndef RUN_ZONE_HPP
#define RUN_ZONE_HPP

#include "FencePointer.hpp"

class Zone {
private:
    int min;
    int max;
    long start_pos;
    long end_pos;

public:
    Zone(int min, int max, long start_pos, long end_pos) {
        FencePointer fencePointer_(min, max);
        this->min = min;
        this->max = max;
        this->start_pos = start_pos;
        this->end_pos = end_pos;
    }

    int getMin() const {
        return this->min;
    }

    int getMax() const {
        return this->max;
    };

    bool query(int key) {
        return key <= this->max && key >= this->min;
    }

    long getStartPos() const {
        return start_pos;
    }

    long getEndPos() const {
        return end_pos;
    }
};

#endif //RUN_ZONE_HPP
