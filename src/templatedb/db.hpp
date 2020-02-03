#ifndef _TEMPLATEDB_DB_H_
#define _TEMPLATEDB_DB_H_

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "templatedb/operation.hpp"

namespace templatedb
{

class Operation;


class Value
{
public:
    std::vector<int> items;

    Value() {};
    Value(std::vector<int> _items) {items = _items;};

    bool operator ==(Value const & other) const
    {
        return (items == other.items);
    };
};


class DB
{
public:
    std::unordered_map<int, Value> table;

    DB() {};

    Value get(int key);
    void put(int key, Value val);
    std::vector<Value> scan();
    std::vector<Value> scan(int min_key, int max_key);
    void del(int key);
    size_t size();

    std::vector<Value> execute_op(Operation op);

    bool load_from_file(std::string file_name);
};

}   // namespace templatedb

#endif /* _TEMPLATEDB_DB_H_ */