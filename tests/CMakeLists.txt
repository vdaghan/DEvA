cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(DEvA_Tests CXX)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG 703bd9caab50b139428cea1aaff9974ebee5742e) # release-1.10
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
message(STATUS "gtest_SOURCE_DIR = ${gtest_SOURCE_DIR}")

add_executable(DEvA_Tests "DEvA_Variation.cpp")
target_include_directories(DEvA_Tests PUBLIC "../include")
target_include_directories(DEvA_Tests PUBLIC "${gtest_SOURCE_DIR}/googletest/include")
target_link_libraries(DEvA_Tests PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(DEvA_Tests TEST_PREFIX DEvA_Tests::)
