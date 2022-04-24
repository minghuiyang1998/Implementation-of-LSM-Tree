#ifndef RUN_FP_HPP
#define RUN_FP_HPP

#include <string>
#include<algorithm>

class FencePointer {
public:
    FencePointer() {
        min = INT32_MAX;
        max = INT32_MIN;
    }

    FencePointer(int min_, int max_) {
        min = min_;
        max = max_;
    }

    bool query(int key) {
        return key <= max && key >= min;
    }

    void program(int key) {
        max = std::max(key, max);
        min = std::min(key, min);
    }

    int getMin() const {
        return min;
    }

    int getMax() const {
        return max;
    };
private:
    int min;
    int max;
};
#endif //RUN_FP_HPP