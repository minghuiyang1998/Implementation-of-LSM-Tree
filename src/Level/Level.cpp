#include "Level.hpp"

Level::Level() {}

Level::Level(int num, int threshold) {
    levelNum = num;
    compactionType = Leveling;
    threshold = threshold;

    runs = vector<Run>();
}

Level::Level(int num, CompactionType type, int threshold) {
    levelNum = num;
    compactionType = type;
    threshold = threshold;

    runs = vector<Run>();
}

Level::~Level() {}

void Level::addARun(Run r) {
    runs.push_back(r);
}
