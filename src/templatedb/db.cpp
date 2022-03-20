#include "db.hpp"

using namespace templatedb;


Value DB::get(int key)
{
    if (table.count(key))
        return table[key];
    
    return Value(false);
}


void DB::put(int key, Value val)
{
    table[key] = val;

    // 1. put in memory table
    // 2. check if mmtable need to be cleaned
    // 3, clean() will return a SSTable and put this SSTable into the first level
}


std::vector<Value> DB::scan()
{
    std::vector<Value> return_vector;
    for (auto pair: table)
    {
        return_vector.push_back(pair.second);
    }

    return return_vector;
}


std::vector<Value> DB::scan(int min_key, int max_key)
{
    std::vector<Value> return_vector;
    for (auto pair: table)
    {
        if ((pair.first >= min_key) && (pair.first <= max_key))
            return_vector.push_back(pair.second);
    }

    return return_vector;
}


void DB::del(int key)
{
    table.erase(key);
}


void DB::del(int min_key, int max_key)
{
    for (auto it = table.begin(); it != table.end(); ) {
        if ((it->first >= min_key) && (it->first <= max_key)){
            table.erase(it++);
        } else { 
            ++it;
        }
    }
}


size_t DB::size()
{
    return table.size();
}


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

bool DB::buildLevels() {
    // first load config.txt
    
}

bool DB::load_all_files() {

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
        Run r = Run(); // TODO: need to wirte constructors and pass arguments later

        int l_num = stoi(readLine);
        Level level = this->levels.getLevelVector(l_num);
        level.addARun(r);
    } else {
        fprintf(stderr, "Unable to read run file %s", fname.c_str());
        return false;
    }
    return true;
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
        std::getline(file, readLine); // First line is 50
        this->mmtableThreshold = stoi(readLine);

        // construct all levels
        for(int i = 0; i < this->totalLevels; i++) {
            Level newLevel = Level(i, this->levelsThreshold[i]);
            this->levels.setLevel(i, newLevel);
        }


        // while (std::getline(file, line))
        // {
        //     std::stringstream linestream(line);
        //     std::string item;

        //     std::getline(linestream, item, ',');
        //     key = stoi(item);
        //     std::vector<int> items;
        //     while(std::getline(linestream, item, ','))
        //     {
        //         items.push_back(stoi(item));
        //     }
        //     this->put(key, Value(items));
        //     if (value_dimensions == 0)
        //         value_dimensions = items.size();
        // }
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
    // before close the database, clean the memory table, create an SSTable and store in a file. call clean() 
    //
    if (file.is_open())
    {
        this->write_to_file();
        file.close();
    }
    this->status = CLOSED;

    return true;
}


bool DB::write_to_file()
{
    file.clear();
    file.seekg(0, std::ios::beg);

    std::string header = std::to_string(table.size()) + ',' + std::to_string(value_dimensions) + '\n';
    file << header;
    for(auto item: table)
    {
        std::ostringstream line;
        std::copy(item.second.items.begin(), item.second.items.end() - 1, std::ostream_iterator<int>(line, ","));
        line << item.second.items.back();
        std::string value(line.str());
        file << item.first << ',' << value << '\n';
    }

    return true;
}