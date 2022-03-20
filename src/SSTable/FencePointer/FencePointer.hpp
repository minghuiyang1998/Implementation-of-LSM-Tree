#include <string>
#include<algorithm>

class FencePointer {
public:
    FencePointer();;
    void program(std::string key) {
        max = std::max(key, max);
        min = std::min(key, min);
    };
private:
    std::string min;
    std::string max;
};