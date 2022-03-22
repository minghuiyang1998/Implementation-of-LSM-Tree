#include "db.hpp"
#include "../Run/Run.hpp"

using namespace templatedb;

Value DB::get(int key) {
    // 1. memoryTable search
    Value res = memoryTable.query(key);
    if (!res.items.empty()) {
        return res;
    }
    // 2. search in levels

    return Value(false);
}

void DB::put(int key, Value val) {
    // TODO: fullfill all the attriutes in val
    val.setTimestamp(timestamp + 1);
    timestamp += 1;
    // 1. put in memory table
    memoryTable.insert(key, val);
    if(memoryTable.getMapSize() > mmtableThreshold) {
        // 2. check if memory-table need to be cleaned
        std::map<int, Value> data = memoryTable.clean();
        // TODO: create new run and add to first level, create a new file
        int size = data.size();
        int level = 1;
        std::string filepath;
        sprintf(filepath, "../../Storage/Data/%d.txt", generatorCount);
        generatorCount++;
        Run newRun = Run(size, level, filepath, data);
        write_to_file(level, size, filepath, data);

        // add new run to level
        if(compactionType == Leveling) {
            compactLeveling(newRun);
        } else {
            compactTiering(newRun);
        }
    }
}

bool DB::write_to_file(int level, int size, std::string filepath, std::map<int, Value> data) {
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
}


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
    for (auto it = table.begin(); it != table.end(); ) {
        if ((it->first >= min_key) && (it->first <= max_key)){
            table.erase(it++);
        } else { 
            ++it;
        }
    }
}

// bool DB::buildLevels(std::vector<int> levels) {

//     return false;
// }

// bool DB::load_all_sst() {
//     std::vector<std::string> sstList = get_sst_list();
//     for (auto sst_file : sstList) {
//     }
//     return false;
// }

 std::vector<std::string> DB::get_file_list() {
     std::vector<std::string> res;
     for (const auto & entry : std::__fs::filesystem::directory_iterator("../../Storage/Data"));
         res.push_back(entry.path());
 }

 std::string DB::make_filename(const std::string& name,
                          uint64_t number,
                          const char* suffix) {
     char buf[100];
     snprintf(buf, sizeof(buf), "/%06llu.%s",static_cast<unsigned long long>(number),suffix);
     return name + buf;
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

bool DB::load_data_file(std::string & fname) // load a datafile, one file store one run
{
    // std::ifstream fid(fname);
    // if (fid.is_open())
    // {
    //     int key;
    //     int line_num = 0;
    //     std::string line;
    //     std::getline(fid, line); // First line is rows, col
    //     while (std::getline(fid, line))
    //     {
    //         line_num++;
    //         std::stringstream linestream(line);
    //         std::string item;

    //         std::getline(linestream, item, ' ');
    //         std::string op_code = item;

    //         std::getline(linestream, item, ' ');
    //         key = stoi(item);
    //         std::vector<int> items;
    //         while(std::getline(linestream, item, ' '))
    //         {
    //             items.push_back(stoi(item));
    //         }
    //         this->put(key, Value(items));
    //     }
    // }
    // else
    // {
    //     fprintf(stderr, "Unable to read %s\n", fname.c_str());
    //     return false;
    // }
    std::ifstream fid(fname);
    if(fid.is_open()) {
        std::string readLine;
        std::getline(fid, readLine);
        int l_num = stoi(readLine);
        std::getline(fid, readLine);
        int size = stoi(readLine);
        map<int, Value> data = load_data(fname);

        Run r = Run(size, l_num, fname, data);
        
        Level level = this->levels.getLevelVector(l_num);
        level.addARun(r);
    } else {
        fprintf(stderr, "Unable to read run file %s", fname.c_str());
        return false;
    }
    return true;
}

bool parsebool(std::string str) {
    if(str == "true") return true;
    else return false;
}

map<int, Value> DB::load_data(std::string & fname) {
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



db_status DB::open(std::string & fname)    // open config.txt, set initial attributes
{
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
        this->levelsThreshold = vector<int>(this->totalLevels-1);
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

        // construct all levels
        for(int i = 0; i < this->totalLevels; i++) {
            Level newLevel = Level(i, this->levelsThreshold[i]);
            this->levels.setLevel(i, newLevel);
        }

        // get all the runs file path and load runs in memory
        vector<std::string> allFilePath = get_file_list();
        while(std::string s: allFilePath) {
            loed_data_file(s);
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


bool DB::close()
{
    // before close the database, clean the memory table, create an Run and store in a file. call clean()
    //
    if (file.is_open())
    {
        this->write_to_file();
        file.close();
    }
    this->status = CLOSED;

    return true;
}

void DB::compactLeveling(Run run) {
    int curr = 1;
    while (curr <= levels.getTotalSize()) {
        Level curr_level = levels.getLevelVector(curr);
        if (curr_level.size() == 0) {
            curr_level.addARun(run);
            break;
        } else { //number of run == 1
            Run temp = curr_level.removeARun();
            std::map<int, Value> res = temp.readDisk(); // previous
            std::map<int, Value> run_data = run.readDisk();

            for (const auto& element : run_data) { // use new_data override prev_data
                int key = element.first;
                Value val = element.second;
                // exist key, override directly
                // exist: if it is tombstone: Pass it to the bottom and make sure everything before this time point is deleted
                // no exist key, push
                res[key] = val;
            }

            // TODO: delete sst of temp from disk
            // TODO: path = write_to_file(res);
            int run_size = res.size();
            Run newRun = Run(run_size, curr, "", res);
            curr_level.addARun(newRun);

            if (run_size <= curr_level.getThreshold()
            || curr_level.getThreshold() <= -1) {
                break;
            }
        }
        curr += 1;
    }

}

void DB::compactTiering(Run run) {
    int curr = 1;
    Level curr_level = levels.getLevelVector(curr);
    curr_level.addARun(run);
    while (curr_level.size() > curr_level.getThreshold()
    || curr_level.getThreshold() <= -1) {
        std::map<int, Value> res;
        // from old to new, new run always inserted to the end of the level
        for (int i = 0; i < curr_level.size(); i++) {
            Run curr = curr_level.getARun(i);
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

        // TODO: delete sst of temp from disk
        // TODO: path = write_to_file(res);
        int run_size = res.size();
        Run newRun = Run(run_size, curr, "", res);
        levels.getLevelVector(curr + 1).addARun(newRun);
        curr += 1;
        curr_level = levels.getLevelVector(curr);
    }
}
