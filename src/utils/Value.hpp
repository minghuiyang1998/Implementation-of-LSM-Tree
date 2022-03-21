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
    Value(bool _visible, int _timestamp, bool _isTombstone, std::vector<int> _items) {
        visible = _visible;
        timestamp = _timestamp;
        isTombstone = _isTombstone;
        items = _items;
    }

    bool operator ==(Value const & other) const
    {
        return (visible == other.visible) && (items == other.items);
    }
};