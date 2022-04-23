#ifndef _DELETE_TABLE_H_
#define _DELETE_TABLE_H_

#include <cstdio>
#include "utils/Value.hpp"
#include <vector>
#include <map>
#include "Record.hpp"

class DeleteTable {
private:
    std::vector<Record> records;

public:
    void put(Record record);

    /**
     * if true, key is legal
     * if false, key is illegal
     */
    bool filterSingleQuery(int key, Value val);

    /**
     * return a same size vector of keys
     * if true, key is legal
     * if false, key is illegal
     */
    std::map<int, bool> filterRangeQuery(std::map<int, Value> keyValues);

    const std::vector<Record> &getRecords() const;
};

#endif /* _DELETE_TABLE_H_ */
