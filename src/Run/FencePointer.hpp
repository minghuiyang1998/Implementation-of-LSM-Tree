#include <string>
#include<algorithm>

class FencePointer {
public:
    FencePointer() {
        min = INT32_MAX;
        max = INT32_MIN;
    };
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