#include "DeleteTable.hpp"

void DeleteTable::put(Record record) {

}

bool DeleteTable::filterSingleQuery(int key) {
    // iterate this->records
    for (auto iter = this->records.begin(); iter != this->recors.end(); ++iter){
        if (iter->minKey <= key && key <= iter->maxKey){
            // key is illegal
            return false;
        }
    }
    return true;
}

vector<bool> DeleteTable::filterRangeQuery(std::vector<int> keys) {
    vector<bool> nums(keys.size());
    for(int i = 0; i < keys.size(); ++i){
        nums[i] = filterSingleQuery(keys[i]);
    }
    return nums;
}

