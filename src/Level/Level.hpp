#include "../SSTable/Run.hpp"
#include <vector>

typedef enum Level_Type_ {
    Tiering,
    Leveling,
} CompactionType;

class Level {
private: 
    int levelNum;
    CompactionType compactionType;
    int threshold;
    vector<Run> runs;
public:
    Level();
    Level(int levelNum, CompactionType type, int threshold);
    Level(int levelNum, int threshold);
    ~Level();

    void addARun(Run r);
};