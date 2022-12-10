cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(DEvA_BUILD_TESTS TRUE CACHE BOOL "Build tests for DEvA")

include(FetchContent)

project(DEvA LANGUAGES CXX VERSION 0.5.0.5)
configure_file(deva_version.cpp.in deva_version.cpp @ONLY)

add_custom_target(Flowchart COMMAND dot -Tsvg ${CMAKE_CURRENT_BINARY_DIR}/DEvA.dot -o ${CMAKE_CURRENT_BINARY_DIR}/DEvA.svg)

#FetchContent_Declare(lua GIT_REPOSITORY https://github.com/lua/lua.git GIT_TAG 5d708c3f9cae12820e415d4f89c9eacbe2ab964b SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/Lua ) # release-5.4.4
#FetchContent_GetProperties(lua)
#if(NOT lua_POPULATED)
#	FetchContent_Populate(lua)
#	message(STATUS "lua_SOURCE_DIR = ${lua_SOURCE_DIR}")
#	message(STATUS "lua_BINARY_DIR = ${lua_BINARY_DIR}")
#endif()
#project(Lua C)
#add_library(Lua ${lua_SOURCE_DIR}/onelua.c)
#target_include_directories(Lua PUBLIC ${lua_SOURCE_DIR})
FetchContent_Declare(json GIT_REPOSITORY https://github.com/nlohmann/json.git GIT_TAG bc889afb4c5bf1c0d8ee29ef35eaaf4c8bef8a5d GIT_PROGRESS TRUE SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/json) # release-3.11.2
FetchContent_MakeAvailable(json)

set(DEvA_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/deva_version.cpp"
				 "${CMAKE_CURRENT_SOURCE_DIR}/src/Bibliography.cpp"
				 "${CMAKE_CURRENT_SOURCE_DIR}/src/RandomNumberGenerator.cpp"
				 "${CMAKE_CURRENT_SOURCE_DIR}/src/Strategy.cpp")

add_library(DEvA ${DEvA_SOURCES})
add_library(DEvA::DEvA ALIAS DEvA)
target_compile_features(DEvA PUBLIC cxx_std_23)
#target_link_libraries(DEvA PRIVATE DEvA_version)
#target_precompile_headers(DEvA PRIVATE ${lua_SOURCE_DIR})
target_include_directories(DEvA PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(DEvA PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(DEvA PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/BuildingBlocks")
target_link_libraries(DEvA nlohmann_json::nlohmann_json)
target_precompile_headers(DEvA PRIVATE <map> <string> <vector>)

if (DEvA_BUILD_TESTS)
	add_subdirectory("tests")
endif()
add_subdirectory("examples")
