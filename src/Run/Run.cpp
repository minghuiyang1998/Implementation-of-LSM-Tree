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
    bool isInBF = isInBloomFilter(key); // false positive
    bool isInFP = isInFencePointer(key);
    // not in bloom filter or fence pointer, definitely not in this run
    if (!isInBF || !isInFP) return Value(false);

    // Identify the parts that might contain data
    std::map<int, Value> blocks;
    for (const auto& zone : zones) {
        if (zone.getMin() <= key && key <= zone.getMax()) {
            std::map<int, Value> map = readDisk(start_pos, end_pos);;
            blocks.insert(map.begin(), map.end());
        }
    }

    // return value
    bool isKey = blocks.count(key);
    if (isKey) {
        return blocks[key];
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
    // Identify the parts that might contain data
    std::map<int, Value> blocks;
    for (const auto& zone : zones) {
        // read block, if overlap
        if (max_key < zone.getMin() && zone.getMax() < min_key) continue;
        std::map<int, Value> map = readDisk(start_pos, end_pos);;
        blocks.insert(map.begin(), map.end());
    }

    for (const auto& element : blocks) {
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

bool Run::parsebool(std::string str) {
    if(str == "true") return true;
    else return false;
}

// TODO: change to read in block
std::map<int, Value> Run::readDisk(int start_pos, int end_pos) {
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

/**
 * recover from SST
 * */
Run::Run(Metadata metadata) {
    this->size = metadata.getSize();
    this->level = metadata.getLevel();
    this->filePath = metadata.getFilePath();
    bloomFilter = BF::BloomFilter(metadata.getBfNumElement(), metadata.getBfBitsPerElement(), metadata.getBfVec());
    fencePointer = FencePointer(metadata.getFpMin(), metadata.getFpMax());
    this->num_zones = num_zones;
    this->num_elements_per_zone = num_elements_per_zone;
    this->zones = zones;
}

/**
 * new Run, init from data and metadata
 * db will get a new metadata later
 * */
Run::Run(Metadata metadata, const map<int, Value> &map) {
    this->size = metadata.getSize();
    this->level = metadata.getLevel();
    this->filePath = metadata.getFilePath();
    int bf_numElement = metadata.getBfNumElement();
    int bf_bitPerElement = metadata.getBfBitsPerElement();

    // new BF & FP
    bloomFilter = BF::BloomFilter(bf_numElement, bf_bitPerElement); // default numElement: 1024,  bitsPerElement: 64
    for (const auto& element : map) {
        std::string key = to_string(element.first);
        bloomFilter.program(key);
        fencePointer.program(element.first);
    }

    // zones
    this->num_zones = metadata.getNumZones();
    this->num_elements_per_zone = metadata.getNumElementsPerZone();
    zones = metadata.getZones();
}

Metadata Run::getInfo() {
    Metadata info(
            bloomFilter.numElement,
            bloomFilter.bitsPerElement,
            bloomFilter.getBfVec(),
            fencePointer.getMin(),
            fencePointer.getMax(),
            filePath,
            level,
            size,
            num_zones,
            num_elements_per_zone,
            zones
            );

    return info;
}



