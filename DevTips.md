1. run example/ or test/ for test
2. target ? library ? in CMakeLists.txt
3. add "#endif " to avoid multiple includes
4. in src/CMakeLists.txt modify cmakelist.txt to make sure the current file imports CPP files
5. how to use examples/simple_benchmark
   benchmark simply takes in two files, a data file and a workload file and
   measures the time it takes to execute the workload over the dataset
6. how to use tests/
    ctest ?
7. Memorytable and Disk searches only for the presence or absence of the key and do not test for visible

how to run gtest -> each function work well ?
______
after midterm:
1. range query
2. range delete
3. data path