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
#include "../utils/Value.hpp"

namespace templatedb {

    typedef enum _status_code {
        OPEN = 0,
        CLOSED = 1,
        ERROR_OPEN = 100,
    } db_status;

    class DB {
        public:
            db_status status;

        DB();

        Value get(int key);
            void put(int key, Value val);
            std::vector<Value> scan();
            std::vector<Value> scan(int min_key, int max_key);
            void del(int key);
            void del(int min_key, int max_key);
            void compactLeveling();
            void compactTiering();
            size_t size();

            db_status open(std::string & fname);
            bool close();
            bool load_data_file(std::string & fname);
            std::vector<Value> execute_op(Operation op);

        virtual ~DB();

    private:
            const std::string db_name;
            std::string manifest_file_name;
            const std::string sst_storage = "./storage/";
            int curr_SST_number = 0;

            std::fstream file;
            std::unordered_map<int, Value> table;
            Levels levels;
            MemoryTable memoryTable;

            size_t value_dimensions = 0;
            bool buildLevels();

            // int baseLevelSize;  // size of first leve

            bool write_to_file();
            bool load_all_sst();
            std::vector<std::string> get_sst_list();
            std::string make_filename(const std::string& name, uint64_t number, const char* suffix);
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