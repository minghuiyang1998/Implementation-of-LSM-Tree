#include "../utils/Value.hpp"
#include "../Run/Run.hpp"
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
    std::map<int, Value> clean();  // first generate an Run, save it in a file, then clear the map
    /**
     * put a <key, Value> in the map, Then determine whether to insert or delete
     */
    void insert(int key, Value val);    // put a key in the map

    Value query(int key);
    std::vector<Value> range_query(int min_key, int max_key);
    void save();  // save sstable in a file
    int getMapSize() {
        return map.size();
    }
    /**
     * check the visible varaible in Value and
     * then determine it it a delete operation
     * @param Key
     */
    void pointDelete(int Key, Value value);
};