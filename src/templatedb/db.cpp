#include <cstdio>
#include <cstring>
#include <utility>
#include <random>

#include "db.hpp"

using namespace templatedb;

// ------------------------------------------------------------------------
//  public function
// ------------------------------------------------------------------------

/** open a database
 *  @param config filename
 *  @return db_status
 * */
db_status DB::open(const std::string & filename)    // open config.txt, set initial attributes
{
    std::string fpath = this->DEFAULT_PATH + "/" + filename;
    this->config_file_path = fpath;
    this->file.open(fpath, std::ios::in | std::ios::out);
    if (file.is_open())
    {
        this->status = OPEN;
        // New file implies empty file
        if (file.peek() == std::ifstream::traits_type::eof())
            return this->status;

        construct_database();
    }
    else if (!file) // File does not exist
    {
        std::string dirname = create_data_dir();
        create_config_file(fpath, dirname);
        this->file.open(fpath, std::ios::in | std::ios::out);
        construct_database();

        this->status = OPEN;
    }
    else
    {
        file.close();
        this->status = ERROR_OPEN;
    }

    return this->status;
}

/** get Value with a key
 *  @param int key
 *  @return if found and Value is visible, return value, otherwise return Value(false)
 * */
Value DB::get(int key) {
    // 1. memoryTable search
    Value res = memoryTable.query(key);
    // if the res doesn't have a timestamp => doesn't find the key
    // if return Value(false) the timestamp will be -1
    if (res.timestamp != -1) {
        // include this key
        // did not deleted by single delete or range delete
        bool isEligible = deleteTable.filterSingleQuery(key, res) && res.visible;
        if (isEligible) {
            return res;
        } else {
            // not eligible, There is no need to look in files,
            // because the latest key here has been deleted
            return Value(false);
        }
    }
    // 2. search in levels
    for (int i = 0; i < levels.getTotalSize(); ++i) {
        Level &level = levels.getLevelVector(i);
        // get all runs in this level
        // search table in level from new to old
        for (int j = level.size() - 1; j >= 0; j--) {
            Run &run = level.getARun(j);
            res = run.query(key);
            // include this key
            if (res.timestamp != -1) {
                // did not deleted by single delete or range delete
                bool isEligible = deleteTable.filterSingleQuery(key, res) && res.visible;
                if (isEligible) {
                    return res;
                } else {
                    // include this key but deleted, There is no need to look in old files
                    return Value(false);
                }
            }
        }
    }

    return Value(false);
}

/** put Value with a key
 *  @param int key, Value val
 * */
void DB::put(int key, Value val) {
    // fulfill all the attributes in val
    val.setTimestamp(timestamp);
    timestamp += 1;
    // 1. put in memory table
    memoryTable.put(key, val);
    if(memoryTable.getMapSize() >= mmtableThreshold) {
        // 2. check if memory-table need to be cleaned
        std::map<int, Value> data = memoryTable.clear();
        // create new run and add to first level, create a new file
        int size = data.size();
        int level = 0;
        std::string run_dir_path = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount);

        Metadata metadata(run_dir_path, level, size);
        // generate zones and wrap up metadata
        vector<Zone> zones = create_zones(data, metadata);
        metadata.setZones(zones);
        metadata.setNumZones(zones.size());
        // generate new run with data and metadata, the generator will fill out BloomFilter info and Fence Pointer info
        Run newRun = Run(metadata, data);

        // get completed metadata with BloomFilter info and FencePinter info
        metadata = newRun.getInfo(); // return Metadata with complete Run info
        // write to SST
        write_files(metadata, data, run_dir_path);

        // add new run to level
        if(compactionType == Leveling) {
            compactLeveling(newRun);
        } else {
            compactTiering(newRun);
        }
    }
}

/** range query, scan Value whose key is between min_key and max_key
 *  @param int min_key, int max_key
 *  @return a vector contains all eligible values
 * */
std::vector<Value> DB::scan(int min_key, int max_key) {
    std::vector<Value> return_vector;

    // 1. memoryTable search (override duplicate keys with the newest value in memoryTable)
    std::map<int, Value> return_map = memoryTable.range_query(min_key, max_key);

    // 2. search in levels
    for (int i = 0; i < levels.getTotalSize(); ++i) {
        Level &level = levels.getLevelVector(i);
        // get all runs in this level
        // search table in this level from new to old,
        for (int j = level.size() - 1; j >= 0; j--) {
            Run &run = level.getARun(j);
            std::map<int, Value> result = run.range_query(min_key, max_key);
            for (const auto& r : result) {
                int key = r.first;
                Value val = r.second;
                // make sure the new one will not be overridden by old one
                // Add only keys that do not exist in the map
                if (return_map.count(key) == 0) {
                    return_map[key] = val;
                }
            }
        }
    }

    // 3. filter all rangeDeleted and not visible
    for (const auto& item : return_map) {
        int key = item.first;
        Value val = item.second;
        bool isEligible = deleteTable.filterSingleQuery(key, val) && val.visible;
        if (isEligible) {
            return_vector.push_back(val);
        }
    }

    return return_vector;
}

/** delete a key
 *  @param int key
 * */
void DB::del(int key) {
    // create new value with visible = false
    Value val = Value(false);
    put(key, val);
}

/** range delete, delete Value whose key is between min_key and max_key
 *  @param int min_key, int max_key
 * */
void DB::del(int min_key, int max_key) {
    Record record(min_key, max_key, timestamp);
    timestamp += 1;
    deleteTable.put(record);
}

/** close database
 * @return bool close successfully or not
 * */
bool DB::close()
{
    // before close the database, clean the memory table, create a Run and store in a file. call clear()
    map<int, Value> data = this->memoryTable.clear();
    int size = data.size();
    int level = 0;
    std::string run_dir_path = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount);

    if (size > 0) {
        Metadata metadata(run_dir_path, level, size);  // initialize metadata
        vector<Zone> zones = create_zones(data, metadata);     // create zones
        metadata.setZones(zones);   // set zones in metadata
        metadata.setNumZones(zones.size());
        Run newRun = Run(metadata, data);   // create a run, in the constructor, we generate BF and FP
        metadata = newRun.getInfo();           // get the complete metadata
        write_files(metadata, data, run_dir_path);    // write metadata and data into files

        // add new run to level
        if(compactionType == Leveling) {
            compactLeveling(newRun);
        } else {
            compactTiering(newRun);
        }
    }

    if (file.is_open())
    {
        update_config_file();     // update the config file(counter, level and timestamp)
        file.close();
    }
    write_delete_table();         // update delete table
    this->status = CLOSED;

    return true;
}

// ------------------------------------------------------------------------
//  private function
// ------------------------------------------------------------------------

/** construct the structure of database in memory*/
void DB::construct_database() {
    // first read config.txt
    // read total number of levels
    std::string readLine;
    std::getline(file, readLine); // First line: total no. of levels
    int totalLevels = stoi(readLine);
    this->levels = Levels(this->firstLevelThreshold);

    // read threshold for the first level
    std::getline(file, readLine); // Second line: first level threshold
    this->firstLevelThreshold = stoi(readLine);

    // read mmtablethreshold
    std::getline(file, readLine); // Third line
    this->mmtableThreshold = stoi(readLine);

    // read compaction type
    std::getline(file, readLine);
    if(readLine == "Leveling") this->compactionType = Leveling;
    else if(readLine == "Tiering") this->compactionType = Tiering;

    // read generator count
    std::getline(file, readLine);
    this->generatorCount = stoi(readLine);

    // read timestamp
    std::getline(file, readLine);
    this->timestamp = stoi(readLine);

    // read data file dir
    std::getline(file, readLine);
    this->data_files_dirname = readLine;

    // construct all levels
    int levelThreshold = firstLevelThreshold;
    for(int i = 0; i < totalLevels; i++) {
        levels.addALevel();
    }

    // read delete table
    load_delete_table();

    // get all the runs file path and load runs in memory
    vector<std::string> allRunDirPath = get_run_dir_list(data_files_dirname);
    for(const std::string& runPath: allRunDirPath) {
        Metadata metadata = load_metadata(runPath + "/metadata");
        Run newRun = Run(metadata);
        Level &level = this->levels.getLevelVector(metadata.getLevel());
        level.addARun(newRun);
    }
}

/** get all the run directories of a database
 * @param dirname the directory name of the database
 * @return all the names of run directories
 **/
 std::vector<std::string> DB::get_run_dir_list(const std::string& dirname) {
    std::string dirpath = DEFAULT_PATH + "/" + dirname;
    std::vector<std::string> res;
    for (const auto & entry : std::__fs::filesystem::directory_iterator(dirpath)){
        std::string entryPath = entry.path();
        if(entryPath == DEFAULT_PATH + "/" + data_files_dirname + "/delete_table")
            continue;
        res.push_back(entry.path());
    }
    return res;
 }

/** Only used in basic_test.cpp. To check the number of visible records in the database*/
std::vector<Value> DB::scan() {
    std::vector<Value> return_vector;
    std::map<int, Value> return_map;

    // 1. search in levels
    for (int i = 0; i < levels.getTotalSize(); ++i) {
        Level &level = levels.getLevelVector(i);
        // get all runs in this level
        // search table in this level from new to old,
        for (int j = level.size() - 1; j >= 0; j--) {
            Run &run = level.getARun(j);
            int min_key = run.getMin();
            int max_key = run.getMax();
            std::map<int, Value> result = run.range_query(min_key, max_key);
            for (const auto& r : result) {
                int key = r.first;
                Value val = r.second;
                // make sure the new one will not be overridden by old one
                if (return_map.count(key) == 0) {
                    return_map[key] = val;
                }
            }
        }
    }

    // 2. all in memorytable
    for (const auto& pair: memoryTable.getMap()) {
        int key = pair.first;
        Value val = pair.second;
        return_map[key] = val;
    }

    // 3. filter all rangeDeleted and not visible
    for (const auto& item : return_map) {
        int key = item.first;
        Value val = item.second;
        bool isEligible = deleteTable.filterSingleQuery(key, val) && val.visible;
        if (isEligible) {
            return_vector.push_back(val);
        }
    }

    return return_vector;
}

/** used in construct_database
 *  load the delete table from the file
 **/
void DB::load_delete_table() {
    std::string filepath = DEFAULT_PATH + "/" + data_files_dirname + "/delete_table";
    std::ifstream fid(filepath);
    if(fid.is_open()) {
        std::string readLine;
        while (std::getline(fid, readLine)) {
            std::stringstream line(readLine);
            std::string str;
            std::getline(line, str, ',');
            int minKey = stoi(str);
            std::getline(line, str, ',');
            int maxKey = stoi(str);
            std::getline(line, str, ',');
            int delete_timestamp = stoi(str);
            Record record(minKey, maxKey, delete_timestamp);
            deleteTable.put(record);
        }
    }
}

/** create zones
 *  @param data the data that need to creat the zone
 *  @param metadata the metadata
 **/
std::vector<Zone> DB::create_zones(const std::map<int, Value> & data, Metadata & metadata) {
    std::vector<Zone> zones;
    int byte_count = 0;
    int min_key, max_key, min_byte, max_byte;
    int count = 0;
    for(const auto& iter: data) {
        // record the value of min key and the lower bound of byte range
        if(count % metadata.getNumElementsPerZone() == 0) {
            min_key = iter.first;
            min_byte = byte_count;
        }
        // record the value of max key and the higher bound of byte range
        if(count % metadata.getNumElementsPerZone() == metadata.getNumElementsPerZone()-1
        || count == data.size()-1) {
            max_key = iter.first;
            max_byte = byte_count;
            Zone zone(min_key, max_key, min_byte, max_byte);
            zones.push_back(zone);
        }
        // calculate the byte
        byte_count += sizeof(int); // bytes of key
        byte_count += sizeof(int); // bytes of size of items
        for(int i = 0; i < iter.second.items.size(); i++) {
            byte_count += sizeof(int);  // bytes of items
        }
        byte_count += sizeof(bool);  // bytes of visible
        byte_count += sizeof(int);  // bytes of timestamp

        count++;
    }
    return zones;
}

/** create config file for database
 *  This function is called when opening a new database
 **/
void DB::create_config_file(const std::string & fpath, const std::string & data_dirname) const {
    const std::string& config_filepath = fpath;
    std::ofstream fd(config_filepath);

    std::string writeLine;
    // write current level number
    writeLine = "0";
    fd << writeLine << endl;
    // write first level threshold
    writeLine = to_string(firstLevelThreshold);
    fd << writeLine << endl;
    // write mmtable threshold
    writeLine = to_string(DEFAULT_MMTABLE_THRESHOLD);
    fd << writeLine << endl;
    //write type;
    if(DEFAULT_TYPE == Tiering)
        writeLine = "Tiering";
    else
        writeLine = "Leveling";
    fd << writeLine << endl;
    // write timestamp and count
    writeLine = "0";
    fd << writeLine << endl;
    fd << writeLine << endl;
    // write data_dir_name
    writeLine = data_dirname;
    fd << writeLine << endl;
    fd.close();
}

/** write to SST
 *  first write metadata in a text file, and then write key value pairs in a binary file
 * */
void DB::write_files(const Metadata& metadata, const std::map<int, Value>& data, const std::string& run_dir_path) {
    create_run_dir(run_dir_path);
    generatorCount++;
    write_metadata(metadata, run_dir_path);
    write_data(data, run_dir_path);
}

/**  create a directory for a Run*/
void DB::create_run_dir(const std::string& run_dir_path) {
    std::__fs::filesystem::create_directories(run_dir_path);
}

/** create a metadata file and write metadata*/
void DB::write_metadata(const Metadata& metadata, const std::string& run_dir_path) {
    std::string filepath = run_dir_path + "/metadata";
    std::ofstream fd(filepath);
    std::string writeLine;
    // write first row
    int bf_numElement = metadata.getBfNumElement();
    writeLine = to_string(bf_numElement);
    fd << writeLine << endl;
    // write second row
    int bf_bitsPerElement = metadata.getBfBitsPerElement();
    writeLine = to_string(bf_bitsPerElement);
    fd << writeLine << endl;

    // write bit vector
    writeLine = "";
    for(bool b: metadata.getBfVec()) {
        if(b) writeLine += "1";
        else writeLine += "0";
    }
    fd << writeLine << endl;

    // write fence pointer
    int fp_min = metadata.getFpMin();
    writeLine = to_string(fp_min);
    fd << writeLine << endl;

    int fp_max = metadata.getFpMax();
    writeLine = to_string(fp_max);
    fd << writeLine << endl;

    // write file path
    writeLine = metadata.getFilePath();
    fd << writeLine << endl;

    int level = metadata.getLevel();
    writeLine = to_string(level);
    fd << writeLine << endl;

    int size = metadata.getSize();
    writeLine = to_string(size);
    fd << writeLine << endl;

    int num_zones = metadata.getNumZones();
    writeLine = to_string(num_zones);
    fd << writeLine << endl;

    int num_elements_per_zone = metadata.getNumElementsPerZone();
    writeLine = to_string(num_elements_per_zone);
    fd << writeLine << endl;

    // write zones
    for(Zone z: metadata.getZones()) {
        writeLine = to_string(z.getMin())
                + "," + to_string(z.getMax())
                + "," + to_string(z.getStartPos())
                + "," + to_string(z.getEndPos());
        fd << writeLine << endl;
    }
    fd.close();
}

/**
 * write data to a data file
 * write key value pairs in binary format
 */
void DB::write_data(const std::map<int, Value>& data, const std::string& run_dir_path) {
    std::string filepath = run_dir_path + "/data";
    std::ofstream fd(filepath, std::ios::binary);
    for(const auto& iter: data) {
        int key = iter.first;
        fd.write((char*)&key, sizeof(int));   // write key
        Value value = iter.second;
        int size = iter.second.items.size();
        fd.write((char*)&size, sizeof(int));   // write size of items
        for(int i = 0; i < size; i++) {
            fd.write((char*)&value.items[i], sizeof(int));  // write every value in items
        }
        fd.write((char*)&value.timestamp, sizeof(int));   // write timestamp
        fd.write((char*)&value.visible, sizeof(bool));     // write visible
    }
    fd.close();
}

/**
 * Write delete table to the delete table file
 * This happens when closing the database
 * The delete table is written in text file
 */
void DB::write_delete_table() {
    std::string filepath = DEFAULT_PATH + "/" + data_files_dirname + "/delete_table";
    std::ofstream fd(filepath);
    std::string writeLine;

    for(Record r: deleteTable.getRecords()) {
        int minKey = r.minKey;
        int maxKey = r.maxKey;
        int delete_timestamp = r.timestamp;

        writeLine = to_string(minKey) + "," + to_string(maxKey) + "," + to_string(delete_timestamp);
        fd << writeLine << endl;
    }
}

/**
 * update config file for database
 * This happens when closing the database
 */
void DB::update_config_file() {
    std::string fpath = this->config_file_path;
    delete_file(fpath);
    std::ofstream file(fpath);
    std::string writeLine;
    // write total no. levels
    int totalLevels = levels.getTotalSize();
    writeLine = to_string(totalLevels);
    file << writeLine << endl;
    // write first level threshold
    writeLine = to_string(this->firstLevelThreshold);
    file << writeLine << endl;
    // write level threshold
    writeLine = to_string(this->mmtableThreshold);
    file << writeLine << endl;
    // leveling or tiering;
    if(this->compactionType == Leveling) {
        writeLine = "Leveling";
    } else {
        writeLine = "Tiering";
    }
    file << writeLine << endl;
    // file count
    writeLine = to_string(this->generatorCount);
    file << writeLine << endl;
    // timestamp
    writeLine = to_string(this->timestamp);
    file << writeLine << endl;
    // data dir
    writeLine = this->data_files_dirname;
    file << writeLine << endl;
}

/** random generates a char in A-Z*/
char random_generator() {
    static std::mt19937 generator(std::random_device{}());
    static std::uniform_int_distribution<std::size_t> distribution('A', 'Z');
    return static_cast<char>(distribution(generator));
}

/** generates a random name for a database directory*/
std::string generate_name() {
    std::string ret;
    for(int i = 0; i < 10; i++) {
        ret += random_generator();
    }
    return ret;
}

/** create a directory for te database data*/
std::string DB::create_data_dir() {
    data_files_dirname = generate_name();
    std::string data_files_path = DEFAULT_PATH + "/" + data_files_dirname;
    std::__fs::filesystem::create_directories(data_files_path);
    return data_files_dirname;
}

/** delete a directory*/
void DB::delete_dir(const std::string & dir_path) {
    std::__fs::filesystem::remove_all(dir_path);
}

/** delete a file*/
void DB::delete_file(const std::string & file_path) {
    std::__fs::filesystem::remove(file_path);
}

/** Leveling compaction*/
void DB::compactLeveling(Run r) {
    Run run = r;
    // always start from level 1
    int curr = 0;
    // initial
    if (levels.getTotalSize() == 0) {
        levels.addALevel();
    }
    while (true) {
        // get current level
        Level &curr_level = levels.getLevelVector(curr);
        std::map<int, Value> res;
        // no Run in current level
        if (curr_level.size() == 0) {
            // the run is derived from upper level, re-create sst with new level number
            res = run.readRun();
            delete_dir(run.getFilePath());
        } else { //number of run == 1
            // curr level is full, need compaction
            Run temp = curr_level.removeARun();
            res = temp.readRun(); // previous
            std::map<int, Value> run_data = run.readRun();
            std::string deleted_path = temp.getFilePath();
            for (const auto &element: run_data) { // use new_data override prev_data
                int key = element.first;
                Value val = element.second;
                // exist key, override directly
                // exist: if it is tombstone: Pass it to the bottom and make sure everything before this time point is deleted
                // no exist key, push
                res[key] = val;
            }
            //  delete sst of temp from disk
            delete_dir(deleted_path);
            delete_dir(run.getFilePath());
        }

        int r_level = curr;
        int r_size = res.size();

        std::string run_dir_path = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount);
        Metadata metadata(run_dir_path, r_level, r_size);
        vector<Zone> zones = create_zones(res, metadata);
        metadata.setZones(zones);
        metadata.setNumZones(zones.size());
        Run newRun = Run(metadata, res);
        metadata = newRun.getInfo();
        write_files(metadata, res, run_dir_path);
        // put to current level
        curr_level.addARun(newRun);

        if (r_size <= curr_level.getThreshold()) {
            break;
        } else {
            // level overflow, ready to write to next level
            run = curr_level.removeARun();
            // next level doesn't exist
            if (curr + 1 >= levels.getTotalSize()) {
                levels.addALevel();
            }
        }
        curr += 1;
    }
}

/** Tiering compaction*/
void DB::compactTiering(Run run) {
    int curr = 0;
    // initial
    if (levels.getTotalSize() == 0) {
        levels.addALevel();
    }
    levels.getLevelVector(curr).addARun(run);
    while (true) {
        Level &curr_level = levels.getLevelVector(curr);
        if (curr_level.size() > curr_level.getThreshold()) {
            std::map<int, Value> res;
            // from old to new, new run always inserted to the end of the level
            for (int i = 0; i < curr_level.size(); i++) {
                Run &curr = curr_level.getARun(i); // not delete at this time
                std::map<int, Value> curr_map = curr.readRun();
                for (const auto& element : curr_map) { // use new_data override prev_data
                    int key = element.first;
                    Value val = element.second;
                    // exist key, override directly
                    // exist: if it is tombstone: Pass it to the bottom and make sure everything before this time point is deleted
                    // no exist key, push
                    res[key] = val;
                }
            }

            // clean all after merge all sst in this level
            std::vector<std::string> deleted_paths = curr_level.cleanAllRuns();
            // delete all sst from disk
            for(const std::string& s: deleted_paths) {
                delete_dir(s);
            }

            // add to next level
            // ensure we have next level
            if (curr + 1 >= levels.getTotalSize()) {
                levels.addALevel();
            }
            // 2. generate new File
            int r_level = curr + 1;
            int r_size = res.size();
            std::string run_dir_path = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount);

            Metadata metadata(run_dir_path, r_level, r_size);
            vector<Zone> zones = create_zones(res, metadata);
            metadata.setZones(zones);
            metadata.setNumZones(zones.size());
            Run newRun = Run(metadata, res);

            metadata = newRun.getInfo();
            write_files(metadata, res, run_dir_path);

            levels.getLevelVector(r_level).addARun(newRun);
            // move to next level
            curr += 1;
        } else {
            break;
        }
    }
}

// ------------------------------------------------------------------------
//  functions for tests
// ------------------------------------------------------------------------

/** used in simple_benchmark.cpp*/
std::vector<Value> DB::execute_op(Operation op)
{
    std::vector<Value> results;
    if (op.type == GET)
    {
        results.push_back(this->get(op.key));
    }
    else if (op.type == PUT)
    {
        this->put(op.key, Value(op.args));
    }
    else if (op.type == SCAN)
    {
        results = this->scan(op.key, op.args[0]);
    }
    else if (op.type == DELETE)
    {
        if ( op.args.size()>0 ){
            this->del(op.key, op.args[0]);
        }
        else
            this->del(op.key);
    }

    return results;
}

/**
 * Load metadata of a Run
 * This happens after opening the database and need to load the metadata in memory
 * Read as a text file
 */
Metadata DB::load_metadata(const std::string &fpath) {
    std::ifstream fid(fpath);
    if (fid.is_open()) {
        std::string readLine;
        std::getline(fid, readLine);
        int bf_numElement = stoi(readLine);
        std::getline(fid, readLine);
        int bf_bitsPerElement = stoi(readLine);
        std::getline(fid, readLine);
        std::vector<bool> bf_vec;
        for(char i : readLine) {
            if(i == '0') {
                bf_vec.push_back(false);
            } else {
                bf_vec.push_back(true);
            }
        }
        std::getline(fid, readLine);
        int fp_min = stoi(readLine);
        std::getline(fid, readLine);
        int fp_max = stoi(readLine);
        std::getline(fid, readLine);
        string filePath = readLine;
        std::getline(fid, readLine);
        int level = stoi(readLine);
        std::getline(fid, readLine);
        int size = stoi(readLine);
        std::getline(fid, readLine);
        int num_zones = stoi(readLine);
        std::getline(fid, readLine);
        int num_elements_per_zone = stoi(readLine);
        std::vector<Zone> zones;
        for(int i = 0 ; i < num_zones; i++) {
            std::getline(fid, readLine);
            std::stringstream line(readLine);
            std::string str;
            std::getline(line, str, ',');
            int min = stoi(str);
            std::getline(line, str, ',');
            int max = stoi(str);
            std::getline(line, str, ',');
            int start_pos = stoi(str);
            std::getline(line, str, ',');
            int end_pos = stoi(str);
            Zone zone(min, max, start_pos, end_pos);
            zones.push_back(zone);
        }
        Metadata metadata(bf_numElement, bf_bitsPerElement, bf_vec, fp_min, fp_max,
                          filePath, level, size, num_zones, num_elements_per_zone, zones);
        return metadata;
    }
    Metadata metadata;
    return metadata;
}

/** load benchmark test data file*/
bool DB::load_benchmark_test_data_file(std::string & fname) {
    std::ifstream fid(fname);
    if (fid.is_open())
    {
        int key;
        int line_num = 0;
        std::string line;
        std::getline(fid, line); // First line is rows, col
        while (std::getline(fid, line))
        {
            line_num++;
            std::stringstream linestream(line);
            std::string item;

            std::getline(linestream, item, ',');
            key = stoi(item);
            std::vector<int> items;
            while(std::getline(linestream, item, ','))
            {
                items.push_back(stoi(item));
            }
            this->put(key, Value(items));
        }
    }
    else
    {
        fprintf(stderr, "Unable to read %s\n", fname.c_str());
        return false;
    }

    return true;
}

/** return map size*/
int DB::size() {
    return memoryTable.getMapSize();
}
