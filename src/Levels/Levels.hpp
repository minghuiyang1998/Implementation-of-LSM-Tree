#ifndef _LSM_TREE_LEVELS_H_
#define _LSM_TREE_LEVELS_H_

#include "../Level/Level.hpp"
#include "vector"

class Levels {
    vector<Level> levelVector;
    int totalSize;    // number of plies(levels)
};

#endif /* _LSM_TREE_LEVELS_H_ */
