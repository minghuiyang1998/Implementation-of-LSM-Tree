# Usage & Test Of LSM
## persistence test - test persistence (gtest)
run "main()" in test
## basic test - test basic operations in 3 dbs  (gtest)
run "main()" in test
## BloomFilter test - test bloom filter
run "main()" in test
## simple benchmark - test workload time
dim: the dimension is the length of "items" of "Value".
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
exp: benchmark_db.txt -f ../../data/test_100_2.data -w ../../data/test_25_2_200.wl (relate to build/examples/simple_benchmark)
```

4. Modify attributes value of the database
Compaction type: db.hpp line67
First level threshold: db.hpp line68
Memory table maximum size: db.hpp line69
Number of elements per zone: db.hpp line 18
BF bits per element: Metadata.hpp line10
