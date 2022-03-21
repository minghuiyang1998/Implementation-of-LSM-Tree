#include "Level.hpp"

Level::Level() {}

Level::Level(int num, int threshold) {
    levelNum = num;
    threshold = threshold;

    runs = vector<Run>();
}

Level::~Level() {}

void Level::addARun(Run r) {
    runs.push_back(r);
}

int Level::size() {
    return runs.size();
}

Run Level::removeARun() {
    //TODO: impl
}

int Level::getThreshold() const {
    return threshold;
}

Run Level::getARun(int index) {
    //TODO:
}

