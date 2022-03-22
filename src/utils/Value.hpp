#ifndef LSM_TREE_VALUE_H
#define LSM_TREE_VALUE_H

#include <vector>

class Value {
public:
    std::vector<int> items;
    bool visible = true;
    long timestamp;

    Value() {}
    Value(bool _visible) {visible = _visible;}
    Value(std::vector<int> _items) { items = _items;}
    Value(bool _visible, int _timestamp, std::vector<int> _items) {
        visible = _visible;
        timestamp = _timestamp;
        items = _items;
    }

    bool operator ==(Value const & other) const {
        return (visible == other.visible) && (items == other.items);
    }

    void setTimestamp(int _timestamp) {
        timestamp = _timestamp;
    }

    void setVisible(bool _visible) {
        visible = _visible;
    }

    long getTimestamp() const {
        return timestamp;
    }

    bool isVisible() const {
        return visible;
    }
};

#endif /* LSM_TREE_VALUE_H */