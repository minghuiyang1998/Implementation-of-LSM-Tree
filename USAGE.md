# Usage & Test Of LSM
## persistence test - test persistence
run "main()" in test
## basic test - test basic operations in 3 dbs 
run "main()" in test
## simple benchmark - test workload time
1. generate datafile with "tools/gen_data.py"
2. generate datafile with "tools/gen_workload.py"
3. run main() with configuration
    <dbname> -f <datafilename> -w <workloadfilename>