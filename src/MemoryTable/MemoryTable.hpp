#include "../utils/Value.hpp"
#include "SSTable/SSTable.hpp"
#include <map>

class MemoryTable {
private:
    map<int, Value> map;

public:
    SSTable clean();  // first generate an SSTable, save it in a file, then clear the map
    void put();    // put a key in the map

    void save();  // save sstable in a file


};