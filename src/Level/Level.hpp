#ifndef _LSM_TREE_LEVEL_H_
#define _LSM_TREE_LEVEL_H_

#include "SSTable/SSTable.hpp"
#include <vector>

class Level {
private: 

    vector<SSTable> sstables;
};

#endif /* _LSM_TREE_LEVEL_H_ */