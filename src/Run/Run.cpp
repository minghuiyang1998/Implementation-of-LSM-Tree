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
            long start_pos = zone.getStartPos();
            long end_pos = zone.getEndPos();
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

std::map<int, Value> Run::range_query(int min_key, int max_key) {
    int fp_min = fencePointer.getMin();
    int fp_max = fencePointer.getMax();
    // not available element in this sst
    if (min_key > fp_max || max_key < fp_min) return {};

    std::map<int, Value> results;
    // Identify the parts that might contain data
    std::map<int, Value> blocks;
    for (const auto& zone : zones) {
        // read block, if overlap
        if (max_key < zone.getMin() || zone.getMax() < min_key) continue;
        long start_pos = zone.getStartPos();
        long end_pos = zone.getEndPos();
        std::map<int, Value> map = readDisk(start_pos, end_pos);;
        blocks.insert(map.begin(), map.end());
    }

    for (const auto& element : blocks) {
        int key = element.first;
        if (key <= fp_max && key >= min_key) {
            Value val = element.second;
            results[key] = val;
        }
    }
    return results;
}

// change to read in block
std::map<int, Value> Run::readDisk(long start_pos, long end_pos) {
    std::string run_data_path = this->filePath + "/data";
    std::ifstream fid(run_data_path, ios::binary);
    map<int, Value> ret;
    if(fid.is_open()) {
        long current_byte = start_pos;
        fid.seekg(start_pos, ios::beg);
        while(true) {
            int key;
            fid.read((char*)&key, sizeof(int)); // read key
            current_byte += sizeof(int);
            Value value;
            int items_size;
            fid.read((char*)&items_size, sizeof(int)); // read items size
            current_byte += sizeof(int);
            for(int j = 0; j < items_size; j++) {
                int t;
                fid.read((char*)&t, sizeof(int)); // read every item
                current_byte += sizeof(int);
                value.items.push_back(t);
            }
            int timestamp;
            fid.read((char*)&timestamp, sizeof(timestamp));
            current_byte += sizeof(timestamp);
            value.timestamp = timestamp;
            bool visible;
            fid.read((char*)&visible, sizeof(visible));
            current_byte += sizeof(visible);
            value.visible = visible;
            ret[key] = value;

            if(current_byte > end_pos) break;
        }
    } else {
        fprintf(stderr, "Unable to read run file %s", run_data_path.c_str());
    }
    return ret;
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
    bloomFilter = BF::BloomFilter(metadata.getBfNumElement(), metadata.getBfBitsPerElement(), metadata.getBfVec());;
    fencePointer = FencePointer(metadata.getFpMin(), metadata.getFpMax());
    this->num_zones = metadata.getNumZones();
    this->num_elements_per_zone = metadata.getNumElementsPerZone();
    this->zones = metadata.getZones();
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

std::map<int, Value> Run::readRun() {
    std::string run_data_path = this->filePath + "/data";
    std::ifstream fid(run_data_path, ios::binary);
    map<int, Value> ret;
    if(fid.is_open()) {
        for(int i = 0; i < this->size; i++) {
            int key;
            fid.read((char*)&key, sizeof(int)); // read key
            Value value;
            int items_size;
            fid.read((char*)&items_size, sizeof(int)); // read items size
            for(int j = 0; j < items_size; j++) {
                int t;
                fid.read((char*)&t, sizeof(int));  //read every item
                value.items.push_back(t);
            }
            int timestamp;
            fid.read((char*)&timestamp, sizeof(timestamp));
            value.timestamp = timestamp;
            bool visible;
            fid.read((char*)&visible, sizeof(bool));
            value.visible = visible;
            ret[key] = value;
        }
    } else {
        fprintf(stderr, "Unable to read run file %s", run_data_path.c_str());
    }
    return ret;
}

int Run::getMin() const {
    return fencePointer.getMin();
}

int Run::getMax() const {
    return fencePointer.getMax();
};


