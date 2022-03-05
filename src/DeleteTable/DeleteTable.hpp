#include "stdio.h"
#include <vector>

class DeleteTable {
private:
    std::vector<std::pair<int, int>> deletedRange;

public:
    bool inTable(int key);


};