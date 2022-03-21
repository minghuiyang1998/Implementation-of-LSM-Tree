//
// Created by Yang Minghui on 2022/3/19.
//

#ifndef TEMPLATEDB_TABLEREADER_HPP
#define TEMPLATEDB_TABLEREADER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "utils/Value.hpp"

typedef enum file_status {
    OPEN = 0,
    CLOSED = 1,
    ERROR_OPEN = 100,
} file_status;

/**
 * use json format here
 * */
class TableReader {
public:
    file_status status;

    // for SST initial
    file_status readConfig() {
        // TODO:
    }

    // for SST query
    file_status readData() {
        // TODO:
    }

private:
    std::fstream file;
    std::unordered_map<int, Value> table;
    size_t value_dimensions = 0;
    std::string fname;
    int config_offset = 5;
};

#endif //TEMPLATEDB_TABLEREADER_HPP
