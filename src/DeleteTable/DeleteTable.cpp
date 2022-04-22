#include "DeleteTable.hpp"

void DeleteTable::put(Record record) {

}

Value DeleteTable::filterSingleQuery(Value result) {
    return Value(false);
}

std::vector<Value> DeleteTable::filterRangeQuery(std::vector<Value> results) {
    return std::vector<Value>();
}

