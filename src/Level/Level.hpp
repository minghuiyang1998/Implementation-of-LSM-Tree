#ifndef _LSM_TREE_LEVEL_H_
#define _LSM_TREE_LEVEL_H_

#include "Run/Run.hpp"
#include <vector>

class Level {
private: 
    vector<Run> sstables;
};

#endif /* _LSM_TREE_LEVEL_H_ */