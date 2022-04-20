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
#include <cstdio>

#include "operation.hpp"
#include "../Level/Level.hpp"
#include "../Levels/Levels.hpp"
#include "../MemoryTable/MemoryTable.hpp"
#include "../Run/Run.hpp"
#include "../utils/Value.hpp"
#include "../DeleteTable/DeleteTable.hpp"
#include "../DeleteTable/Record.hpp"

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
    
    db_status open(const std::string & fname);
    bool close();

    std::vector<Value> execute_op(Operation op);

private:
    const CompactionType DEFAULT_TYPE = Leveling;
    const int DEFAUlT_LEVEL_THRESHOLD = 3;
    const int DEFAULT_MMTABLE_THRESHOLD = 30;
    const std::string DEFAULT_PATH = "../../Storage";

    std::string data_files_dirname;
    std::fstream file;
    Levels levels;
    MemoryTable memoryTable;
    DeleteTable deleteTable;

    CompactionType compactionType;
    size_t value_dimensions = 0;
    int totalLevels;
    int generatorCount;
    int timestamp = 0;
    int firstLevelThreshold;
    int mmtableThreshold;

    std::vector<std::string> get_file_list(const std::string& dirname);
    void create_config_file(const std::string & fpath, const std::string & data_dirname) const;
    std::string create_data_dir();
    bool load_data_file(const std::string & dirpath, const pair<int, int> &pair);
    std::pair<int, int> load_metadata(const std::string & fpath);
    std::string write_files(int level, int size, std::map<int, Value> data);
    void create_run_dir();
    void write_metadata(int level, int size);
    void write_data(const std::map<int, Value>& data);

    void delete_file(const std::string & fname);
    void update_config_file(const std::string & fname);

    void construct_database();

};

}   // namespace templatedb

#endif /* _TEMPLATEDB_DB_H_ */
