#ifndef _LSM_TREE_LEVEL_H_
#define _LSM_TREE_LEVEL_H_

#include "../Run/Run.hpp"
#include <vector>

class Level {
private: 
    int levelNum;
    int threshold;
    std::vector<Run> runs;
public:
    Level();
    Level(int levelNum, int threshold);
    ~Level();

    void addARun(Run r);
    Run removeARun();
    Run &getARun(int index);
    std::vector<std::string> cleanAllRuns();
    int getThreshold() const;
    int size();

    int getLevelNum() const;
};

#endif /* _LSM_TREE_LEVEL_H_ */
