#ifndef _LSM_TREE_LEVEL_H_
#define _LSM_TREE_LEVEL_H_

#include "../Run/Run.hpp"
#include "../templatedb/db.hpp"
#include <vector>

using namespace templatedb;
class Level {
private: 
    int threshold;
    vector<Run> runs;
public:
    Level();
    Level(int levelNum, int threshold);
    ~Level();

    void addARun(Run r);
    Run removeARun();
    Run getARun(int index);
    void cleanAllRuns();
    int getThreshold() const;
    int size();
};

#endif /* _LSM_TREE_LEVEL_H_ */
