#include <string>
#include <sstream>
#include <fstream>

#include "FencePointer/FencePointer.hpp"
#include "BloomFilter/BloomFilter.hpp"

class Run
{
private:
    /* data */
    BloomFilter bloomFilter;
    FencePointer fencePointer;
    std::fstream file;
    string id;
    int size;
    int level;
    
public:
    Run() {} ;
    ~Run();
};

