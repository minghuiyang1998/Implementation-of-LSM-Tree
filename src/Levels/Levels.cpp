#include "Levels.hpp"

Levels::Levels() {}

Levels::Levels(int firstLevelThreshold) {
    this -> firstLevelThreshold = firstLevelThreshold;
}

Levels::~Levels() {}

void Levels::setLevel(int num, Level l) {
    levelVector[num] = l;
}

Level &Levels::getLevelVector(int l) {
    return levelVector[l];
}

void Levels::setLevelVector(int l, const Level& level) {
    levelVector[l] = level;
}

int Levels::getTotalSize() const {
    return levelVector.size();
}

void Levels::addALevel() {
    int newLevelNumb = levelVector.size();
    // T, T^2, T^3, T^4.....
    int newThreshold = pow(firstLevelThreshold, newLevelNumb);
    Level newLevel(newLevelNumb, newThreshold);
    levelVector.push_back(newLevel);
}
