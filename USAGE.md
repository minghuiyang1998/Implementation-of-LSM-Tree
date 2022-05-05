# Usage & Test Of LSM
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