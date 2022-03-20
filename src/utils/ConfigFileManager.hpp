//
// Created by Yang Minghui on 2022/3/19.
//

#ifndef TEMPLATEDB_TABLEMANAGER_HPP
#define TEMPLATEDB_FILEMANAGER_HPP

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Value.hpp"

typedef enum file_status {
    OPEN = 0,
    CLOSED = 1,
    ERROR_OPEN = 100,
} file_status;

/**
 * work for db initialization
 * key is a string, value is also a string
 * return a map of key-value set
 * adapt to db-config <key, int> and Run-format <key, Value>
 * */

class ConfigFileManager {
public:
    file_status status;

    file_status readFile(std::string & fname) {
        this->file.open(fname, std::ios::in | std::ios::out);
        if (file.is_open()) {
            this->status = OPEN;
            // New file implies empty file
            if (file.peek() == std::ifstream::traits_type::eof())
                return {};

            std::string line;
            std::getline(file, line); // First line is rows, col
            while (std::getline(file, line)) {
                std::stringstream linestream(line);
                std::string item;

                std::vector<std::string> kv;
                while(std::getline(linestream, item, ',')) {
                    kv.push_back(item);
                }
                table[kv[0]] = kv[1];
            }
        } else if (!file) { // File does not exist
            this->file.open(fname, std::ios::out);
            this->status = OPEN;
        } else {
            file.close();
            this->status = ERROR_OPEN;
        }
    }

    bool closeFile() {
        // before close the database, clean the memory table, create an Run and store in a file. call clean()
        if (file.is_open()) {
            this->write_to_file();
            file.close();
        }
        this->status = CLOSED;
        return true;
    }

    bool write_to_file() {
        file.clear();
        file.seekg(0, std::ios::beg);
        for(auto item: table) {
            file << item.first << ':' << item.second << '\n';
        }
        return true;
    }

    const std::unordered_map<std::string, std::string> &getTable() const {
        return table;
    }

private:
    std::fstream file;
    std::unordered_map<std::string, std::string> table;
};

#endif //TEMPLATEDB_TABLEMANAGER_HPP
