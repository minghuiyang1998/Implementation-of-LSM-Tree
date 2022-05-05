1. run example/ or test/ for test
2. add "#endif " to avoid multiple includes
3. in src/CMakeLists.txt modify cmakelist.txt to make sure the current file imports CPP files
4. Memorytable and Disk searches only for the presence or absence of the key and do not test for visible