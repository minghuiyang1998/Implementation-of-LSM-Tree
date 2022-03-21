#include "../utils/Value.hpp"
#include "Run/Run.hpp"
#include <map>

class MemoryTable {
private:
    map<int, Value> map;

public:
    std::map<int, Value> clean();  // first generate an Run, save it in a file, then clear the map
    void insert(int key, Value val);    // put a key in the map

    Value query(int key);
    std::vector<Value> range_query(int min_key, int max_key);
    void save();  // save sstable in a file
    int getMapSize() {
        return map.size();
    }
};