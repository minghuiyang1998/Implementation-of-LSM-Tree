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
gen_workload.py <rows> <dim_per_value> <max_key> <folder>
```
2. generate datafile with "tools/gen_workload.py"
```bash
gen_workload.py <rows> <dim_per_value> <max_key> <folder>
```
3. run main() with configuration
    <dbname> -f <datafilename> -w <workloadfilename>