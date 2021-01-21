cmake_minimum_required(VERSION 3.11)

project(knn)

# optimization for dev environments
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# project version
set(MAJOR 0)
set(MINOR 1)
set(REMAP_VERSION ${MAJOR}.${MINOR})

# CXX and C versions
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS "-pthread")

# cmake modules path spec
list(APPEND CMAKE_MODULE_PATH "$ENV{CMAKE_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# cmake binary path spec
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# clang-tidy
set(CMAKE_CXX_CLANG_TIDY
    clang-tidy -p pwd -header-filter=.*
    --checks=-*,readability-*,performance-*)

# --checks=clang-analyzer-*)

# build shared library (on)
option(BUILD_SHARED_LIBS "Build Shared Library" ON)

# project paths spec
set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(INCLUDE_DIR ${PROJECT_DIR}/include)
set(SRC_DIR ${PROJECT_DIR}/src)

find_package(gflags REQUIRED)
find_package(glog REQUIRED)

# define project target
add_executable(knn
    ${SRC_DIR}/io.cpp
    ${SRC_DIR}/workers.cpp
    ${SRC_DIR}/point.cpp
    ${SRC_DIR}/main.cpp
    )

target_include_directories(knn PRIVATE
    ${INCLUDE_DIR}
    )

target_link_libraries(knn
    glog
    gflags
    )

# graph and colorize '*.csv' data before and after clustering
option(GRAPH "view graphs" ON)

if(GRAPH)
    add_custom_command(TARGET knn
        POST_BUILD
        COMMAND ./build/bin/knn --logtostderr=1
        COMMAND /usr/bin/python3 ./scripts/graph.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
endif()
