#include "FencePointer/FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"
#include "string"

class Run
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
    Run(/* args */);
    ~Run();
};

