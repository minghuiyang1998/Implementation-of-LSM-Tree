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
    // always remove the first element(oldest one)
    return runs.erase(runs.begin())[0];
}

int Level::getThreshold() const {
    return threshold;
}

Run Level::getARun(int index) {
    return runs[index];
}

void Level::cleanAllRuns() {
    runs.clear();
}

