#ifndef _LSM_TREE_LEVELS_H_
#define _LSM_TREE_LEVELS_H_

#include "../Level/Level.hpp"
#include "vector"

class Levels {
private:
    std::vector<Level> levelVector;
    int totalSize;    // number of plies(levels)
public:
    Levels();
    Levels(int size);
    ~Levels();

    void setLevel(int num, Level l);
    Level getLevelVector(int l);
    void setLevelVector(int l, const Level& level);

    int getTotalSize() const;
};

#endif /* _LSM_TREE_LEVELS_H_ */
