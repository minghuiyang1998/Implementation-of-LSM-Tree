cmake_minimum_required(VERSION 3.12)
project(templatedb VERSION 0.1.0)

set(templatedb_lib
    src/templatedb/db.cpp
    src/templatedb/operation.cpp
)

add_library(${PROJECT_NAME} ${templatedb_lib})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)

include_directories(external)

add_subdirectory(example)
