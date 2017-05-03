cmake_minimum_required(VERSION 3.7)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
# For example: cmake -Dtest=ON
option(test "Build All Test." OFF)  # Make boolean 'test' available

set(PROJECT_NAME JsonCpp)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

################################
# Normal Libraries & Executables
################################
add_library(json_lib json.cpp)

set(SOURCE_FILES main.cpp)
add_executable(JsonCpp ${SOURCE_FILES})

target_link_libraries(JsonCpp json_lib)

################################
# Testing
################################
if (test)
    # only for apple computer
#    if (APPLE)
#        add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
#        add_definitions(-D__GLIBCXX__)
#    endif (APPLE)

    # add sub directory which has  'project gtest'
    add_subdirectory(lib/gtest-1.8.0)

    enable_testing()

    # Include the gtest library. gtest_SOURCE_DIR is available due to
    # 'project(gtest)' above
    include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

    #############
    # Unit Tests
    #############
    add_executable(runUnitTests test/test_json.cpp)

    # Standard linking to gtest stuff
    target_link_libraries(runUnitTests gtest gtest_main)

    # Extra linking for the project
    target_link_libraries(runUnitTests json_lib)

    add_test(NAME that-test-I-made COMMAND runUnitTests)

    # This is so you can do 'make test' to see all your tests run, instead of
    # manually running the executable runUnitTests to see those specific tests.

    add_test(that-other-test-I-made runUnitTests)

    # You can also omit NAME and COMMAND. The second argument could be some other
    # test executable.
    add_test(that-other-test-I-made runUnitTests)

    add_test(ResetToZero runUnitTests)

endif ()