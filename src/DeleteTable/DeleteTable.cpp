#include "DeleteTable.hpp"

void DeleteTable::put(Record record) {
    records.push_back(record);
}

/**
 * if deleted in range delete, return false.
 * if not found, return true.
 * */
bool DeleteTable::filterSingleQuery(int key, Value val) {
    // iterate records backwards to only compare with newer Records
    for (int i = records.size() - 1; i >= 0; i--) {
        Record record = records[i];
        // record is newer than value
        if (record.timestamp > val.timestamp) {
            // value recorded inside this record, check if is deleted
            if (record.minKey <= key && key <= record.maxKey) {
                return false;
            }
        } else {
            // value is newer
            break;
        }
    }
    return true;
}

const std::vector<Record> &DeleteTable::getRecords() const {
    return records;
}

std::map<int, bool> DeleteTable::filterRangeQuery(std::map<int, Value> keyValues) {
    std::map<int, bool> results;
    for(const auto& elem : keyValues){
        int key = elem.first;
        Value val = elem.second;
        results[key] = filterSingleQuery(key, val);
    }
    return results;
}

DeleteTable::DeleteTable(const std::vector<Record> &records) {
    this->records = records;
}
