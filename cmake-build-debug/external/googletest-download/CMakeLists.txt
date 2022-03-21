cmake_minimum_required(VERSION 2.8.2)
project(googletest-download NONE)

include(ExternalProject)
ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
    SOURCE_DIR "/Users/shun/Desktop/CS561/final_project/561-final-project/cmake-build-debug/external/googletest-src"
    BINARY_DIR "/Users/shun/Desktop/CS561/final_project/561-final-project/cmake-build-debug/external/googletest-build"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    TEST_COMMAND ""
)

