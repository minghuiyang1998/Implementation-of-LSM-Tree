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

        std::string filepath = write_to_file(level, size, data);
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

// TODO: after midterm
void DB::del(int min_key, int max_key) {
    for (auto it = table.begin(); it != table.end(); ) {
        if ((it->first >= min_key) && (it->first <= max_key)){
            table.erase(it++);
        } else { 
            ++it;
        }
    }
}

 std::vector<std::string> DB::get_file_list() {
     std::vector<std::string> res;
     for (const auto & entry : std::__fs::filesystem::directory_iterator("../../Storage/Data")) 
        res.push_back(entry.path());
     return res;
 }

size_t DB::size() {
    return table.size();
}

// used in basic_test.cpp
std::vector<Value> DB::scan() {
    std::vector<Value> return_vector;
    for (auto pair: table) {
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

bool DB::load_data_file(const std::string & fname) // load a datafile, one file store one run
{
    std::ifstream fid(fname);
    if(fid.is_open()) {
        std::string readLine;
        std::getline(fid, readLine);
        int l_num = stoi(readLine);
        std::getline(fid, readLine);
        int size = stoi(readLine);
        map<int, Value> data = load_data(fname);

        Run r = Run(size, l_num, fname, data);
        Level &level = this->levels.getLevelVector(l_num);
        level.addARun(r);

//        this->levels.setLevelVector(l_num, level);
    } else {
        fprintf(stderr, "Unable to read run file %s", fname.c_str());
        return false;
    }
    return true;
}


std::string DB::write_to_file(int level, int size, std::map<int, Value> data) {
    std::string filepath;
    filepath = "../../Storage/Data/";
    filepath += to_string(generatorCount);
    filepath += ".txt";
    generatorCount++;
    std::ofstream file(filepath);

    std::string writeLine;
    // write first row, level
    writeLine = to_string(level);
    file << writeLine << endl;
    // write second row, size
    writeLine = to_string(size);
    file << writeLine << endl;
    for(auto iter: data) {
        writeLine = "";
        writeLine += to_string(iter.first);  // key
        Value v = iter.second;
        bool visible = v.visible;
        if(visible) writeLine += ",true";
        else writeLine += ",false";

        int timestamp = v.timestamp;
        writeLine += ",";
        writeLine += to_string(timestamp);

        std::vector<int> items = v.items;
        for(auto i: items) {
            writeLine += ",";
            writeLine += to_string(i);
        }

        file << writeLine << endl; // write a key/value row
    }
    return filepath;
}


void DB::update_config_file(const std::string & fname) {
    delete_file(fname);
    std::ofstream file(fname);
    std::string writeLine;
    writeLine = to_string(this->totalLevels);
    file << writeLine << endl;

    writeLine = "";
    for(auto iter: this->levelsThreshold) {
        writeLine += to_string(iter);
        if(iter != this->levelsThreshold[this->levelsThreshold.size()-1]) {
            writeLine += ",";
        }
    }
    file << writeLine << endl;

    writeLine = to_string(this->mmtableThreshold);
    file << writeLine << endl;

    if(this->compactionType == Leveling) {
        writeLine = "Leveling";
    } else {
        writeLine = "Tiering";
    }
    file << writeLine << endl;

    writeLine = to_string(this->generatorCount);
    file << writeLine << endl;

    writeLine = to_string(this->timestamp);
    file << writeLine << endl;
}

bool parsebool(std::string str) {
    if(str == "true") return true;
    else return false;
}

map<int, Value> DB::load_data(const std::string & fname) {
    std::ifstream fid(fname);
    std::string readLine;
    map<int, Value> ret;
    int linecount = 0;
    if(fid.is_open()) {
        while(std::getline(fid, readLine)) {
            int key, timestamp;
            bool visible;
            vector<int> items = vector<int>();
            if(linecount == 0 || linecount == 1) {
                linecount++;
                continue;  // skip first two rows
            }
            std::stringstream valuestream(readLine);
            std::string str;
            int itemcount = 0;
            while(std::getline(valuestream, str, ',')) {
                if(itemcount == 0) key = stoi(str);
                else if(itemcount == 1) {
                    visible = parsebool(str);
                }
                else if (itemcount == 2) timestamp = stoi(str);
                else {
                    items.push_back(stoi(str));
                }
                itemcount++;
            }
            linecount++;
            Value v = Value(visible, timestamp, items);
            ret[key] = v;
        }
    } else {
        fprintf(stderr, "Unable to read run file %s", fname.c_str());
    }
    return ret;
}

db_status DB::open(const std::string & filename)    // open config.txt, set initial attributes
{

    std::string fname = this->default_path + filename;
    this->file.open(fname, std::ios::in | std::ios::out);
    if (file.is_open())
    {
        this->status = OPEN;
        // New file implies empty file
        if (file.peek() == std::ifstream::traits_type::eof())
            return this->status;

        // read total number of levels
        std::string readLine;
        std::getline(file, readLine); // First line is 4
        this->totalLevels = stoi(readLine);
        this->levels = Levels(this->totalLevels);

        // read threshold for every level
        std::getline(file, readLine); // Second line is 1,3,5
        std::stringstream levelstream(readLine);
        std::string levelThresholdStr;
        this->levelsThreshold = vector<int>();
        while(std::getline(levelstream, levelThresholdStr, ',')) {
            this->levelsThreshold.push_back(stoi(levelThresholdStr));
        }

        // read mmtablethreshold
        std::getline(file, readLine); // Third line is 50
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

        // construct all levels
        for(int i = 0; i < this->totalLevels; i++) {
            Level newLevel = Level(i, this->levelsThreshold[i]);
            this->levels.setLevel(i, newLevel);
        }

        // get all the runs file path and load runs in memory
        vector<std::string> allFilePath = get_file_list();
        for(std::string s: allFilePath) {
            load_data_file(s);
        }
    }
    else if (!file) // File does not exist
    {
        this->file.open(fname, std::ios::out);
        this->status = OPEN;
    }
    else
    {
        file.close();
        this->status = ERROR_OPEN;
    }

    return this->status; 
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
        std::string filepath = write_to_file(level, size, data);
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
    int curr = 1;
    while (curr <= levels.getTotalSize()) {
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
        std::string filepath = write_to_file(r_level, r_size, res);
        Run newRun = Run(r_size, r_level, filepath, res);
        // put to current level
        curr_level.addARun(newRun);

        if (r_size <= curr_level.getThreshold()
        || curr_level.getThreshold() <= -1) {
            break;
        } else {
            // level overflow, ready to write to next level
            run = curr_level.removeARun();
        }

        curr += 1;
    }

}

void DB::compactTiering(Run run) {
    int curr = 1;
    Level &curr_level = levels.getLevelVector(curr);
    curr_level.addARun(run);
    while (curr_level.getThreshold() != -1 && curr_level.size() > curr_level.getThreshold()) {
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
        int r_level = curr + 1;
        int r_size = res.size();
        std::string filepath = write_to_file(r_level, r_size, res);
        Run newRun = Run(r_size, r_level, filepath, res);
        levels.getLevelVector(r_level).addARun(newRun);
        // move to next level
        curr += 1;
        curr_level = levels.getLevelVector(curr);
    }
}
