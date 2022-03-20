#include "../utils/Value.hpp"
#include "../SSTable/Run.hpp"
#include <map>

class MemoryTable {
private:
    map<int, Value> map;

public:
    Run clean();  // first generate an SSTable, save it in a file, then clear the map
    void put();    // put a key in the map

    void save();  // save sstable in a file


};