#ifndef LSM_TREE_MEM_H
#define LSM_TREE_MEM_H

#include "../utils/Value.hpp"
#include "../Run/Run.hpp"
#include <map>

class MemoryTable {
private:
    map<int, Value> map;
public:
    /**
     * return the map and then clear it
     * @return
     */
    std::map<int, Value> clear();  // first generate an Run, save it in a file, then clear the map
    /**
     * put a <key, Value> in the map, Then determine whether to insert or delete
     */
    void put(int key, Value val);    // put a key in the map

    Value query(int key);
    std::map<int, Value> range_query(int min_key, int max_key);

    int getMapSize() {
        return map.size();
    }
};

#endif /* LSM_TREE_MEM_H */
