1. run example/ or test/ for test
2. target ? library ? in CMakeLists.txt
3. add "#endif " to avoid multiple includes
4. in src/CMakeLists.txt modify cmakelist.txt to make sure the current file imports CPP files
5. how to use examples/simple_benchmark
   benchmark simply takes in two files, a data file and a workload file and
   measures the time it takes to execute the workload over the dataset
6. how to use tests/
    ctest ?

how to run gtest -> each function work well ?