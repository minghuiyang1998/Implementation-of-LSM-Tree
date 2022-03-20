#include "../Level/Level.hpp"
#include "vector"

class Levels {
private:
    vector<Level> levelVector;
    int totalSize;    // number of plies(levels)
public:
    Levels();
    Levels(int size);
    ~Levels();

    void setLevel(int num, Level l);
    Level getLevelVector(int l);
};