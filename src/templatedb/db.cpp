#include "db.hpp"

using namespace templatedb;


Value DB::get(int key) {
    if (table.count(key))
        return table[key];
    
    return Value(false);
}


void DB::put(int key, Value val) {
    table[key] = val;

    // 1. put in memory table
    // 2. check if mmtable need to be cleaned
    // 3, clean() will return a Run and put this Run into the first level
}

std::vector<Value> DB::scan() {
    std::vector<Value> return_vector;
    for (auto pair: table)
    {
        return_vector.push_back(pair.second);
    }

    return return_vector;
}


std::vector<Value> DB::scan(int min_key, int max_key) {
    std::vector<Value> return_vector;
    for (auto pair: table)
    {
        if ((pair.first >= min_key) && (pair.first <= max_key))
            return_vector.push_back(pair.second);
    }

    return return_vector;
}


void DB::del(int key) {
    table.erase(key);
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

void DB::init(std::string & fname) {
    // TODO: read config file
    std::unordered_map<string, string> db_config = {};
    db_name = db_config["name"];
    sst_storage = db_config["sst_storage"];
    // TODO: init Level here
    // TODO: init MemoryTable here
    MMTableThreshold = stoi(db_config["MMTableThreshold"]);
    curr_sst_number = stoi(db_config["curr_sst_number"]);
    manifest_file_name = fname;
}

DB::~DB() {
    close();
}

bool DB::buildLevels() {
    // first load config.txt
    return false;
}

bool DB::load_all_sst() {
    std::vector<std::string> sstList = get_sst_list();
    for (auto sst_file : sstList) {
        // TODO: new SST with config in file
    }
    return false;
}

std::vector<std::string> DB::get_sst_list() {
    std::vector<std::string> res;
    for (const auto & entry : std::__fs::filesystem::directory_iterator(sst_storage))
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

// used in simple_benchmark.cpp
bool DB::load_data_file(std::string & fname)
{
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

            std::getline(linestream, item, ' ');
            std::string op_code = item;

            std::getline(linestream, item, ' ');
            key = stoi(item);
            std::vector<int> items;
            while(std::getline(linestream, item, ' '))
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


// used in persistence_test.cpp && simple_benchmark.cpp
db_status DB::open(std::string & fname)
{
    this->file.open(fname, std::ios::in | std::ios::out);
    if (file.is_open())
    {
        this->status = OPEN;
        // New file implies empty file
        if (file.peek() == std::ifstream::traits_type::eof())
            return this->status;

        int key;
        std::string line;
        std::getline(file, line); // First line is rows, col
        while (std::getline(file, line))
        {
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
            if (value_dimensions == 0)
                value_dimensions = items.size();
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