//
// Created by Yang Minghui on 2022/3/19.
//

#ifndef TEMPLATEDB_DATAFILEMANAGER_HPP
#define TEMPLATEDB_DATAFILEMANAGER_HPP

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
 * work for SSTable / db initialization
 * T1 is the type of key, T2 is the type of value
 * return a map of key-value set
 * adapt to db-config <key, int> and SSTable-format <key, Value>
 * */
class DataFileManager {
public:
    file_status status;

    file_status openFile(std::string & fname) {
        this->file.open(fname, std::ios::in | std::ios::out);
        if (file.is_open()) {
            this->status = OPEN;
            // New file implies empty file
            if (file.peek() == std::ifstream::traits_type::eof())
                return {};

            int key;
            std::string line;
            std::getline(file, line); // First line is rows, col
            while (std::getline(file, line)) {
                std::stringstream linestream(line);
                std::string item;

                std::getline(linestream, item, ',');
                key = stoi(item);
                std::vector<int> items;
                while(std::getline(linestream, item, ','))
                {
                    items.push_back(stoi(item));
                }
                table[key] = Value(items);
                if (value_dimensions == 0)
                    value_dimensions = items.size();
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
        // before close the database, clean the memory table, create an SSTable and store in a file. call clean()
        //
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

    const std::unordered_map<int, Value> &getTable() const {
        return table;
    }

private:
    std::fstream file;
    std::unordered_map<int, Value> table;
    size_t value_dimensions = 0;
};

#endif //TEMPLATEDB_DATAFILEMANAGER_HPP
