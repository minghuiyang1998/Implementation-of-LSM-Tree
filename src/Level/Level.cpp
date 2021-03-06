#include "Level.hpp"

Level::Level() {}

Level::Level(int num, int threshold) {
    levelNum = num;
    this->threshold = threshold;

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
    Run del = runs[0];
    runs.erase(runs.begin());
    return del;
}

int Level::getThreshold() const {
    return threshold;
}

// without & Any changes made to the received object are not reflected in the original vector
Run &Level::getARun(int index) {
    return runs[index];
}

std::vector<std::string> Level::cleanAllRuns() {
    std::vector<std::string> res;
    for (auto & run : runs) {
        res.push_back(run.getFilePath());
    }
    runs.clear();
    return res;
}

int Level::getLevelNum() const {
    return levelNum;
}

