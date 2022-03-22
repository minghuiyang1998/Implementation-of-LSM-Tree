#ifndef _TEMPLATEDB_DB_H_
#define _TEMPLATEDB_DB_H_

#include <fstream>
#include <iostream>
#include <filesystem>
#include <iterator>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "operation.hpp"
#include "../Level/Level.hpp"
#include "../Levels/Levels.hpp"
#include "../MemoryTable/MemoryTable.hpp"
#include "../Run/Run.hpp"
#include "../utils/Value.hpp"

namespace templatedb
{

typedef enum _status_code
{
    OPEN = 0,
    CLOSED = 1,
    ERROR_OPEN = 100,
} db_status;

typedef enum Level_Type_ {
    Tiering,
    Leveling,
} CompactionType;


class DB
{
public:
    db_status status;

    DB() {};
    ~DB() {close();};

    Value get(int key);
    void put(int key, Value val);
    std::vector<Value> scan();
    std::vector<Value> scan(int min_key, int max_key);
    void del(int key);
    void del(int min_key, int max_key);
    void compactLeveling(Run run);
    void compactTiering(Run run);
    size_t size();
    
    db_status open(std::string & fname);
    bool close();

    bool load_all_files();  
    bool load_data_file(std::string & fname);
    bool write_to_file(int level, int size, std::string filepath, std::map<int, Value> data);

    map<int, Value> load_data(std::string & fname);

    std::vector<Value> execute_op(Operation op);

private:
    std::fstream file;
    std::unordered_map<int, Value> table;
    Levels levels;
    MemoryTable memoryTable;
    CompactionType compactionType;

    size_t value_dimensions = 0;
    int totalLevels;
    int generatorCount;
    int timestamp = 0;
    vector<int> levelsThreshold;
    int mmtableThreshold;


    bool buildLevels();
    
    // int baseLevelSize;  // size of first leve
    

    std::vector<std::string> get_file_list();
};

}   // namespace templatedb

#endif /* _TEMPLATEDB_DB_H_ */


/**
 * @brief format of Run
 * metaData:
 * bloomfilter: vector<bool>
 * fence pointer: min, max
 * 
 * Data:
 * key1, [value1], timestamp, tombstone
 * key2, [value2], timestamp, tombstone 
 * ...
 */