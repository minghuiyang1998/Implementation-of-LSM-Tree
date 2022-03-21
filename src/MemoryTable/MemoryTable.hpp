#include "../utils/Value.hpp"
#include "SSTable/SSTable.hpp"
#include <map>

class MemoryTable {
private:
    map<int, Value> map;

public:

    MemoryTable(){
        this->map = map<int, Value> myMap;
    }
    ~MemoryTable() = default;

    MemoryTable(const MemoryTable&) = delete;
    MemoryTable& operator=(const MemoryTable&) = delete;

    /**
     * return the map and then clear it
     * @return
     */
    std::map<int, Value> getMap();

    /**
     * clean the map
     */
    void clear();

    /**
     * put a <key, Value> in the map, Then determine whether to insert or delete
     */
    void put(int Key, Value value);

    Value query(int key);

    vector<Value> rangeQuery(int key_low, int key_high);

    /**
     * check the visible varaible in Value and
     * then determine it it a delete operation
     * @param Key
     */
    void pointDelete(int Key, Value value);




};