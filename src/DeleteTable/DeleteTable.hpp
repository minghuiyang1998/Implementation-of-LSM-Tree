#ifndef _DELETE_TABLE_H_
#define _DELETE_TABLE_H_

#include <cstdio>
#include "utils/Value.hpp"
#include <vector>
#include "Record.hpp"

class DeleteTable {
private:
    std::vector<Record> records;

public:
    void put(Record record);

    Value filterSingleQuery(Value result);

    // 返回合法的结果
    std::vector<Value> filterRangeQuery(std::vector<Value> results);
};

#endif /* _DELETE_TABLE_H_ */
