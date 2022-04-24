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
#include "../utils/Metadata.hpp"
#include "../DeleteTable/DeleteTable.hpp"
#include "../DeleteTable/Record.hpp"
#include "../Run/Zone.hpp"

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

    int size();
    
    db_status open(const std::string & fname);
    bool close();
    bool load_benchmark_test_data_file(std::string & fname);

    std::vector<Value> execute_op(Operation op);

private:
    const CompactionType DEFAULT_TYPE = Leveling;
    int firstLevelThreshold = 3;
    const int DEFAULT_MMTABLE_THRESHOLD = 3;
    const std::string DEFAULT_PATH = "../../Storage";

    std::string data_files_dirname;
    std::string config_file_path;
    std::fstream file;
    Levels levels;
    MemoryTable memoryTable;
    DeleteTable deleteTable;

    CompactionType compactionType;
    size_t value_dimensions = 0;
    int generatorCount;
    int timestamp = 0;
    int mmtableThreshold;

    std::vector<std::string> get_run_dir_list(const std::string& dirname);

    Metadata load_metadata(const std::string & fpath);
    void load_delete_table();

    void create_config_file(const std::string & fpath, const std::string & data_dirname) const;
    std::string create_data_dir();
    void create_run_dir(const std::string& run_dir_path);
    std::vector<Zone> create_zones(const std::map<int, Value> & data, Metadata & metadata);

    void write_files(const Metadata& metadata, const std::map<int, Value>& data, const std::string& run_dir_path);
    void write_metadata(const Metadata& metadata, const std::string& run_dir_path);
    void write_data(const std::map<int, Value>& data, const std::string& run_dir_path);
    void write_delete_table();

    void delete_dir(const std::string & dir_path);
    void delete_file(const std::string & file_path);
    void update_config_file();

    void construct_database();

};

}   // namespace templatedb

#endif /* _TEMPLATEDB_DB_H_ */
