#ifndef RUN_ZONE_HPP
#define RUN_ZONE_HPP

#include "FencePointer.hpp"

class Zone {
private:
    FencePointer fencePointer;
    int start_pos;
    int end_pos;

public:
    Zone(int min, int max, int start_pos, int end_pos) {
        this->start_pos = start_pos;
        this->end_pos = end_pos;
    }

    int getMin() const {
        return fencePointer.getMin();
    }

    int getMax() const {
        return fencePointer.getMax();
    };

    void add(int key) {
        fencePointer.program(key);
    }

    bool query(int key) {
        return key <= fencePointer.getMax() && key >= fencePointer.getMin();
    }

    int getStartPos() const {
        return start_pos;
    }

    int getEndPos() const {
        return end_pos;
    }
};

#endif //RUN_ZONE_HPP
