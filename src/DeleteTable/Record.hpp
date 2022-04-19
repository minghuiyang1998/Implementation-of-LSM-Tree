//
// Created by Yang Minghui on 2022/4/18.
//

#ifndef TEMPLATEDB_RECORD_HPP
#define TEMPLATEDB_RECORD_HPP

class Record {
public:
    int minKey;
    int maxKey;
    int timestamp = -1;

    Record(int minKey, int maxKey, int timestamp) {
        this->timestamp = timestamp;
        this->maxKey = maxKey;
        this->minKey = minKey;
    }
};

#endif //TEMPLATEDB_RECORD_HPP
