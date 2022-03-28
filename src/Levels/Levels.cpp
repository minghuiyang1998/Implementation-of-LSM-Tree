#include "Levels.hpp"

Levels::Levels() {}

Levels::Levels(int size) {
    totalSize = size;
    levelVector = vector<Level>(size);
}

Levels::~Levels() {}

void Levels::setLevel(int num, Level l) {
    levelVector[num] = l;
}

Level Levels::getLevelVector(int l) {
    return levelVector[l - 1];
}

void Levels::setLevelVector(int l, const Level& level) {
    levelVector[l - 1] = level;
}

int Levels::getTotalSize() const {
    return totalSize;
}
