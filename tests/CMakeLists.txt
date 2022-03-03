cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(DEvA_Tests CXX)

enable_testing()

include(FetchContent)

FetchContent_Declare(googletest GIT_REPOSITORY https://github.com/google/googletest.git GIT_TAG e2239ee6043f73722e7aa812a459f54a28552929) # release-1.11
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
message(STATUS "gtest_SOURCE_DIR = ${gtest_SOURCE_DIR}")

add_executable(DEvA_Tests  "DEvA_Parameters.cpp" "DEvA_UserOperators.cpp" "DEvA_Concepts.cpp")
add_executable(DEvA::Tests ALIAS DEvA_Tests)
target_link_libraries(DEvA_Tests PUBLIC DEvA)

target_include_directories(DEvA_Tests PUBLIC "../include")
target_include_directories(DEvA_Tests PUBLIC "${gtest_SOURCE_DIR}/googletest/include")
target_link_libraries(DEvA_Tests PRIVATE gtest_main)
include(GoogleTest)
gtest_discover_tests(DEvA_Tests TEST_PREFIX DEvA::)
