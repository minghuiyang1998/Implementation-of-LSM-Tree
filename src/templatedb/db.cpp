#include <cstdio>
#include <cstring>

#include "db.hpp"

using namespace templatedb;

Value DB::get(int key) {
    // 1. memoryTable search
    Value res = memoryTable.query(key);
    // if the res doesn't have a timestamp => doesn't find the key
    // if return Value(false) the timestamp will be -1
    // could use visible false to judge because a valid tombstone's visible is false
    if (res.timestamp != -1) {
        return res;
    }
    // 2. search in levels
    for (int i = 1; i <= totalLevels; ++i) {
        Level &level = levels.getLevelVector(i);
        // get all runs in this level
        // search from new to old
        for (int j = level.size() - 1; j >= 0; j--) {
            Run &run = level.getARun(j);
            res = run.query(key);
            if (res.timestamp != -1) {
                return res;
            }
        }
    }

    return Value(false);
}

void DB::put(int key, Value val) {
    // fullfill all the attriutes in val
    val.setTimestamp(timestamp + 1);
    timestamp += 1;
    // 1. put in memory table
    memoryTable.put(key, val);
    if(memoryTable.getMapSize() > mmtableThreshold) {
        // 2. check if memory-table need to be cleaned
        std::map<int, Value> data = memoryTable.clear();
        // create new run and add to first level, create a new file
        int size = data.size();
        int level = 1;

        std::string filepath = write_files(level, size, data);
        Run newRun = Run(size, level, filepath, data);

        // add new run to level
        if(compactionType == Leveling) {
            compactLeveling(newRun);
        } else {
            compactTiering(newRun);
        }
    }
}
// TODO: after midterm
std::vector<Value> DB::scan(int min_key, int max_key) {
    std::vector<Value> return_vector;
    // 1. memoryTable search
    // 2. search in levels
    return return_vector;
}

void DB::del(int key) {
    // create new value with visible = false
    Value val = Value(false);
    put(key, val);
}

void DB::del(int min_key, int max_key) {
//    Record record(min_key, max_key, timestamp);
//    timestamp += 1;
//    deleteTable.put(record);
//    for (auto it = table.begin(); it != table.end(); ) {
//        if ((it->first >= min_key) && (it->first <= max_key)){
//            table.erase(it++);
//        } else {
//            ++it;
//        }
//    }
}

void DB::construct_database() {
    // read total number of levels
    std::string readLine;
    std::getline(file, readLine); // First line: total no. of levels
    this->totalLevels = stoi(readLine);
    this->levels = Levels(this->firstLevelsThreshold);

    // read threshold for every level
    std::getline(file, readLine); // Second line: first level threshold
    this->firstLevelsThreshold = stoi(readLine);

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
    for(int i = 0; i < this->totalLevels; i++) {
        Level newLevel = Level(i, (i+1) * firstLevelsThreshold);
        this->levels.setLevel(i, newLevel);
    }

    // get all the runs file path and load runs in memory
    vector<std::string> allDirPath = get_file_list(data_files_dirname);
    for(const std::string& s: allDirPath) {
        pair<int, int> pair = load_metadata(s + "/metadata");
        load_data_file(s, pair);
    }
}

 std::vector<std::string> DB::get_file_list(const std::string& dirname) {
    std::string dirpath = DEFAULT_PATH + "/" + dirname;
    std::vector<std::string> res;
    for (const auto & entry : std::__fs::filesystem::directory_iterator(dirpath))
        res.push_back(entry.path());
    return res;
 }


// used in basic_test.cpp
std::vector<Value> DB::scan() {
    std::vector<Value> return_vector;
    for (const auto& pair: memoryTable.getMap()) {
        return_vector.push_back(pair.second);
    }
    return return_vector;
}

// used in simple_benchmark.cpp
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

std::pair<int, int> DB::load_metadata(const std::string &fpath) {
    std::ifstream fid(fpath);
    if (fid.is_open()) {
        std::string readLine;
        std::getline(fid, readLine);
        int l_num = stoi(readLine);
        std::getline(fid, readLine);
        int size = stoi(readLine);
        return pair<int, int> {l_num, size};
    }
    return pair<int, int> {-1, -1};
}

bool DB::load_data_file(const std::string & dirpath, const pair<int, int> & pair) // load a datafile, one file store one run
{
    std::string fpath = dirpath + "/data";
    // now using binary file read
    std::ifstream fid(fpath, ios::out | ios::binary);
    if(!fid) {
        fprintf(stderr, "Unable to read run file %s", fpath.c_str());
        return false;
    }
    // read pair.second number of values
    map<int, Value> data;
    for(int i = 0; i < pair.second; i++) {
        int key;
        Value value;
        fid.read((char*)&key, sizeof(int));
        fid.read((char*)&value, sizeof(Value));
        data[key] = value;
    }

    Run r = Run(pair.second, pair.first, dirpath, data);
    Level &level = this->levels.getLevelVector(pair.first);
    level.addARun(r);
    return true;
}

void DB::create_config_file(const std::string & fpath, const std::string & data_dirname) const {
    const std::string& config_filepath = fpath;
    std::ofstream fd(config_filepath);

    std::string writeLine;
    // write current level number
    writeLine = "0";
    fd << writeLine << endl;
    // write first level threshold
    writeLine = to_string(DEFAUlT_LEVEL_THRESHOLD);
    fd << writeLine << endl;
    // write mmtable threshold
    writeLine = to_string(DEFAULT_MMTABLE_THRESHOLD);
    fd << writeLine << endl;
    //write type;
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

std::string DB::write_files(int level, int size, std::map<int, Value> data) {
    write_metadata(level, size);
    write_data(data);
    std::string dirpath = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount);
    return dirpath;
}

void DB::write_metadata(int level, int size) {
    std::string filepath = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount) + "/metadata";
    generatorCount++;

    std::ofstream fd(filepath);
    std::string writeLine;
    // write first row, level
    writeLine = to_string(level);
    fd << writeLine << endl;
    // write second row, size
    writeLine = to_string(size);
    fd << writeLine << endl;
    fd.close();
}

void DB::write_data(const std::map<int, Value>& data) {
    std::string filepath = DEFAULT_PATH + "/" + data_files_dirname + "/" + to_string(generatorCount) + "/data";
    std::ofstream fd(filepath, std::ios::binary);
    std::string writeLine;
    for(const auto& iter: data) {
        int key = iter.first;
        Value value = iter.second;
        fd.write((char*)&key, sizeof(int));
        fd.write((char*)&value, sizeof(Value));
    }
    fd.close();
}


void DB::update_config_file(const std::string & fname) {
    delete_file(fname);
    std::ofstream file(fname);
    std::string writeLine;
    // write total no. levels
    writeLine = to_string(this->totalLevels);
    file << writeLine << endl;
    // write first level threshold
    writeLine = to_string(this->DEFAUlT_LEVEL_THRESHOLD);
    file << writeLine << endl;
    // write level threshold
    writeLine = to_string(this->mmtableThreshold);
    file << writeLine << endl;
    // leveling or tiering
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

db_status DB::open(const std::string & filename)    // open config.txt, set initial attributes
{
    std::string fpath = this->DEFAULT_PATH + "/" + filename;
    this->file.open(fpath, std::ios::in | std::ios::out);
    if (file.is_open())
    {
        this->status = OPEN;
        // New file implies empty file
        if (file.peek() == std::ifstream::traits_type::eof())
            return this->status;

        construct_database();
        // read total number of levels
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

// TODO random generates fake random string, need to debug later
std::string generate_name() {
    std::string ret;
    for(int i = 0; i < 10; i++) {
        char c = rand() % (90-65) + 65;
        ret += c;
    }
    return ret;
}

std::string DB::create_data_dir() {
    data_files_dirname = generate_name();
    std::string data_files_path = DEFAULT_PATH + "/" + data_files_dirname;
    std::__fs::filesystem::create_directories(data_files_path);
    return data_files_dirname;
}

void DB::delete_file(const std::string &fname) {
    char filename[fname.size()];
    strcpy(filename, fname.c_str());
    remove(filename);
}

bool DB::close()
{
    // before close the database, clean the memory table, create an Run and store in a file. call clear()
    //
    map<int, Value> data = this->memoryTable.clear();
    int size = data.size();
    int level = 1;

    if (size > 0) {
        std::string filepath = write_files(level, size, data);
        Run newRun = Run(size, level, filepath, data);

        // add new run to level
        if(compactionType == Leveling) {
            compactLeveling(newRun);
        } else {
            compactTiering(newRun);
        }
    }

    if (file.is_open())
    {
        update_config_file("../../Storage/config.txt");
        file.close();
    }
    this->status = CLOSED;

    return true;
}

void DB::compactLeveling(Run r) {
    Run run = r;
    // always start from level 1
    int curr = 1;
    // initial
    if (levels.getTotalSize() == 0) levels.addALevel();
    while (true) {
        // get current level
        Level &curr_level = levels.getLevelVector(curr);
        std::map<int, Value> res;
        // no Run in current level
        if (curr_level.size() == 0) {
            // the run is derived from upper level, re-create sst with new level number
            res = run.readDisk();
            delete_file(run.getFilePath());
        } else { //number of run == 1
            // curr level is full, need compaction
            Run temp = curr_level.removeARun();
            res = temp.readDisk(); // previous
            std::map<int, Value> run_data = run.readDisk();
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
            delete_file(deleted_path);
            delete_file(run.getFilePath());
        }

        int r_level = curr;
        int r_size = res.size();
        std::string filepath = write_files(r_level, r_size, res);
        Run newRun = Run(r_size, r_level, filepath, res);
        // put to current level
        curr_level.addARun(newRun);

        if (r_size <= curr_level.getThreshold()) {
            break;
        } else {
            // level overflow, ready to write to next level
            run = curr_level.removeARun();
            levels.addALevel();
        }

        curr += 1;
    }

}

void DB::compactTiering(Run run) {
    int curr = 1;
    // initial
    if (levels.getTotalSize() == 0) levels.addALevel();
    Level &curr_level = levels.getLevelVector(curr);
    curr_level.addARun(run);
    while (curr_level.size() > curr_level.getThreshold()) {
        std::map<int, Value> res;
        // from old to new, new run always inserted to the end of the level
        for (int i = 0; i < curr_level.size(); i++) {
            Run &curr = curr_level.getARun(i); // not delete at this time
            std::map<int, Value> curr_map = curr.readDisk();
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
            delete_file(s);
        }

        // add to next level
        // 1. add a new level
        levels.addALevel();
        // 2. generate new File
        int r_level = curr + 1;
        int r_size = res.size();
        std::string filepath = write_files(r_level, r_size, res);
        Run newRun = Run(r_size, r_level, filepath, res);
        levels.getLevelVector(r_level).addARun(newRun);
        // move to next level
        curr += 1;
        curr_level = levels.getLevelVector(curr);
    }
}
