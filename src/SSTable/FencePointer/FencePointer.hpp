#include <string>
#include<algorithm>

class FencePointer {
public:
    FencePointer() {
        min = INT_MAX;
        max = INT_MIN;
    };
    bool query(int key) {
        return key <= max && key >= min;
    }
    void program(int key) {
        max = std::max(key, max);
        min = std::min(key, min);
    };
private:
    int min;
    int max;
};