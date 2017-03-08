cmake_minimum_required(VERSION 3.6)
project(json11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/usr/local/include)

#include_directories(json_src)

set(SOURCE_FILES main.cpp json_src/json.h json_src/json.cpp)

add_executable(json11 ${SOURCE_FILES})

target_link_libraries(json11 /usr/local/lib/libgtest.a)