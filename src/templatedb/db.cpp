#include "templatedb/db.hpp"

using namespace templatedb;


Value DB::get(int key)
{
    return table[key];
}


void DB::put(int key, Value val)
{
    table[key] = val;
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
        this->del(op.key);
    }

    return results;
}


bool DB::load_from_file(std::string file_name)
{
    std::ifstream fid(file_name);
    if (fid.is_open())
    {
        int key;
        Value val;
        std::string line;
        std::getline(fid, line); // First line is rows, col
        while (std::getline(fid, line))
        {
            std::stringstream linestream(line);
            std::string item;

            std::getline(linestream, item, ',');
            key = stoi(item);
            val = Value();
            while(std::getline(linestream, item, ','))
            {
                val.items.push_back(stoi(item));
            }
            this->put(key, val);
        }
    }
    else
    {
        fprintf(stderr, "Unable to read %s\n", file_name.c_str());
        return false;
    }

    return true;
}
