#ifndef _DELETE_TABLE_H_
#define _DELETE_TABLE_H_

#include "stdio.h"
#include "utils/Value.hpp"
#include <vector>
#include "Record.hpp"

class DeleteTable {
private:
    std::vector<Record> records;

public:
    void put(Record record);

    /**
     * if true, key is legal
     * if false, key is illegal
     * @param key
     * @return
     */
    bool filterSingleQuery(int key);

    /**
     * return a same size vector of keys
     * if true, key is legal
     * if false, key is illegal
     * @param keys
     * @return
     */
    std::vector<bool> filterRangeQuery(std::vector<int> keys);
};

#endif /* _DELETE_TABLE_H_ */
