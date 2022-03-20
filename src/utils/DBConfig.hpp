//
// Created by Yang Minghui on 2022/3/19.
//

#ifndef TEMPLATEDB_STRUCTURE_H
#define TEMPLATEDB_STRUCTURE_H

#include "string"

class DBConfig {
public:
    // init a empty db
    DBConfig(int currSstNumber,
             const std::string &dbName,
             const std::string &manifestFileName,
             const std::string &storage) {

    }

    // load from storage/xxx.manifest
    DBConfig(int currSSTNumb, const std::string& dbname) {
        curr_SST_number = currSSTNumb;
    }

    std::string MakeFileName(const std::string& name,
                                    uint64_t number,
                                    const char* suffix) {
        char buf[100];
        snprintf(buf, sizeof(buf), "/%06llu.%s",static_cast<unsigned long long>(number),suffix);
        return name + buf;
    }

    std::string manifestFileName() {
        manifest_file_name = db_name + ".db";
        return manifest_file_name;
    }

    int getNextSSTId() {
        curr_SST_number += 1;
        return curr_SST_number;
    }

    std::string SSTFileName(const std::string& name, uint64_t number) {
        return MakeFileName(name, number, "sst");
    }

    int getMmTableThreshold() const {
        return MMTableThreshold;
    }

private:
    int curr_SST_number = 0;
    const std::string db_name;
    std::string manifest_file_name;
    const std::string sst_storage = "./storage/";
    int MMTableThreshold;
    int level_numb;
    vector<int> level_threshold;
};


#endif //TEMPLATEDB_STRUCTURE_H
