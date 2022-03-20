#ifndef LSM_TREE_VALUE_H
#define LSM_TREE_VALUE_H

#include <vector>

class Value
{
public:
    std::vector<int> items;
    bool visible = true;
    int timestamp;
    bool isTombstone;


    Value() {}
    Value(bool _visible) {visible = _visible;}
    Value(std::vector<int> _items) { items = _items;}

    bool operator ==(Value const & other) const
    {
        return (visible == other.visible) && (items == other.items);
    }
};

#endif /* _LSM_TREE_VALUE_H_ */