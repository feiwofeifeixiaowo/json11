cmake_minimum_required(VERSION 3.6)

include_directories(/usr/local/include)

add_executable(runBasicJsonTests basic_check.cpp)

target_link_libraries(runBasicJsonTests gtest gtest_main)

#target_link_libraries(runBasicJsonTests josn)