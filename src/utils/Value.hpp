#ifndef LSM_TREE_VALUE_H
#define LSM_TREE_VALUE_H

#include <vector>
#include <ctime>

class Value {
public:
    std::vector<int> items;
    bool visible = true;
    long timestamp;

    Value(bool _visible) {
        timestamp = std::time(nullptr);
        visible = _visible;
    }
    Value(std::vector<int> _items) {
        timestamp = std::time(nullptr);
        items = _items;
    }

    bool operator ==(Value const & other) const {
        return (visible == other.visible) && (items == other.items);
    }
};

#endif /* LSM_TREE_VALUE_H */