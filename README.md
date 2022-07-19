# Data Systems Architecture - TemplateDB


## Achievements

(read ['final presentation'](https://github.com/minghuiyang1998/Implementation-of-LSM-Tree/blob/master/Presentation_Slides.pdf) for more details)

1. LSM-Tree and database basic implementations: 
   1. Zone(includes Fence Pointer), Run(includes metadata and data file paths), Level, MemoryTable, DeletedList(for RangeDelete)
   2. Leveling and Tiering compaction strategies
   3. Single Query, Range Query, Single Delete, Range Delete, Put
   4. Extend basic_test: add tests for “RangeDelete”
   5. Durable database and compatibility with multiple databases
2. SST data file saved in binary format and read files in blocks
3. Compatibility with multiple databases
4. Perform basic functionality tests, durable tests and different experiments

**The database can finish 3 million insertions and 3 million operations with data-dimension equals to 10 in 8864s (about 2.5h)**


## Requirements

You will need the following on your system (or alternatively develop on the
CSA machines)

    1. CMake
    2. C++ Compiler
    3. GoogleTest (Auto compiled and fetched with CMake)

## Usage

To compile, first create a build directory.

```bash
mkdir build
cd build
```

Afterwards, build using cmake.

```bash
cmake ..
cmake --build .
```

# Test
Tips: Read "README.md" and build the project first.
## persistence test - test persistence (gtest)
/tests/persistence_test.cpp: run "main()" in test
## basic test - test basic operations in 3 dbs  (gtest)
/tests/basic_test.cpp: run "main()" in test
## BloomFilter test - test bloom filter
/examples/bloom_test.cpp: run "main()" in test
## simple benchmark - test workload time
dim: the dimension is the length of "items" of "Value". Used to control per Value size.
1. generate datafile with "tools/gen_data.py"
```bash
gen_data.py <rows> <dim_per_value> <folder>
exp:
cd tools/
python gen_data.py 50 3 ../data
```
2. generate datafile with "tools/gen_workload.py"
```bash
gen_workload.py <rows> <dim_per_value> <max_key> <folder>
exp:
cd tools/
python gen_workload.py 50 3 200 ../data
```
3. run main() with configuration
```bash
<dbname> -f <datafilename> -w <workloadfilename>
open Run/Debug Configuration
exp: benchmark_db.txt -f ../../data/test_100_2.data -w ../../data/test_25_2_200.wl 
(filepaths relate to build/examples/simple_benchmark)
```

4. Modify attributes value of the database (Tone your database in the test)

Tips:
Our parameter is hard coded in the code. So there 2 ways to tone the database. You can copy an existing config file in Storage/ and change the parameter in the config file. And then use this config file.
For non-existing databases, the config file will be created based on the default values in the code. You could change the parameters in our code:

- Compaction type: src/templatedb/db.hpp line67 (use Leveling/Tiering)
- First level threshold. The threshold of a new level = firstLevelThreshold ^ (No. of current level + 1): src/templatedb/db.hpp line68
- Memory table maximum size: src/templatedb/db.hpp line69
- Number of elements per zone: src/utils/Metadata.hpp line 18
- BloomFilter bits per element: src/utils/Metadata.hpp line10

In this way, when it generates a new database that doesn't exist, the value you set will be used as the default parameter

## Directory Description
- src/
- DeleteTable/: relate to Range Delete
- Level/: includes operations in a Level, such as addARun, removeARun....
- Levels/: includes operations of all Levels, such as addALevel..
- MemoryTable/: includes operations of memorytable, such as put, get....
- Run/: includes data structure: Fence Pointer, BloomFilter, Zone. Operations: Put, Get, Delete in a Run.
- templetedb/: db.cpp is the database entry and includes all public APIs.
- utils/: data structures used in databases, including Metadata and Value


- /
- data/: data and workload for benchmark test.
- example/: benchmark_test and bloomfilter_test
- Storage/: all databases and their files including config files and SSTs are stored here.
- tests/: google test.
- tools/: python files used to generate data and workload.
