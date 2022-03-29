// TODO: !!!
// recheck the relative path for all files including /Storage/config.txt, /Storage/Data/..txt, ...

#include "Run.hpp"

bool Run::isInBloomFilter(int key) {
    return bloomFilter.query(to_string(key));
}

bool Run::isInFencePointer(int key) {
    return fencePointer.query(key);
}

Value Run::query(int key) {
    bool isInBF = isInBloomFilter(key);
    bool isInFP = isInFencePointer(key);
    if (!isInBF || !isInFP) return Value(false);

    std::map<int, Value> map = readDisk();
    bool isKey = map.count(key);
    if (isKey) {
        return map[key];
    } else {
        return Value(false);
    }
}

std::vector<Value> Run::range_query(int min_key, int max_key) {
    int fp_min = fencePointer.getMin();
    int fp_max = fencePointer.getMax();
    // not available element in this sst
    if (min_key > fp_max || max_key < fp_min) return {};

    std::vector<Value> results;
    std::map<int, Value> map = readDisk();
    for (const auto& element : map) {
        int key = element.first;
        if (key <= fp_max && key >= min_key) {
            Value val = element.second;
            results.push_back(val);
        }
    }
    return results;
}

void Run::setFilePath(const string &filePath) {
    Run::filePath = filePath;
}

Run::Run(int size, int level, std::string filePath, const std::map<int, Value>& map) {
    this->size = size;
    this->level = level;
    this->filePath = filePath;
    bloomFilter = BF::BloomFilter(1024, 64); // numElement: 1024,  bitsPerElement: 64
    for (const auto& element : map) {
        std::string key = to_string(element.first);
        bloomFilter.program(key);   // seems to have some bugs here
        fencePointer.program(element.first);
    }
}

bool Run::parsebool(std::string str) {
    if(str == "true") return true;
    else return false;
}

std::map<int, Value> Run::readDisk() {
    // copy from db.cpp, use *file to get data
    std::string fname = this->filePath;
    std::ifstream fid(fname);
    std::string readLine;
    map<int, Value> ret;
    int linecount = 0;
    if(fid.is_open()) {
        while(std::getline(fid, readLine)) {
            int key, timestamp;
            bool visible;
            vector<int> items = vector<int>();
            if(linecount == 0 || linecount == 1) {
                linecount++;
                continue;  // skip first two rows
            }
            std::stringstream valuestream(readLine);
            std::string str;
            int itemcount = 0;
            while(std::getline(valuestream, str, ',')) {
                if(itemcount == 0) key = stoi(str);
                else if(itemcount == 1) {
                    visible = parsebool(str);
                }
                else if (itemcount == 2) timestamp = stoi(str);
                else {
                    items.push_back(stoi(str));
                }
                itemcount++;
            }
            linecount++;
            Value v = Value(visible, timestamp, items);
            ret[key] = v;
        }
    } else {
        fprintf(stderr, "Unable to read run file %s", fname.c_str());
    }
    return ret;
    return {};
}

const string &Run::getFilePath() const {
    return filePath;
}

int Run::getLevel() const {
    return level;
}

int Run::getSize() const {
    return size;
}

void Run::setLevel(int level) {
    Run::level = level;
}



