#include "FencePointer/FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "string"

class SSTable
{
private:
    /* data */
    BloomFilter bloomFilter;
    FencePointer fencePointer;
    FILE *fp;
    string id;
    int size;
    int level;
    
public:
    SSTable(/* args */);
    ~SSTable();
};

SSTable::SSTable(/* args */)
{
}

SSTable::~SSTable()
{
}
