cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(DEvA LANGUAGES CXX VERSION 0.0.0.2)
configure_file(deva_version.cpp.in deva_version.cpp @ONLY)

add_custom_target(Flowchart COMMAND dot -Tsvg ${CMAKE_SOURCE_DIR}/doc/flowchart.dot -o ${CMAKE_BINARY_DIR}/flowchart.svg)

set(DEvA_SOURCES "src/Bibliography.cpp"
				 "src/EvolutionaryAlgorithm.cpp"
				 "src/Genealogy.cpp"
				 "src/Generation.cpp"
				 "src/Genotype.cpp"
				 "src/Individual.cpp"
				 "src/Phenotype.cpp"
				 "src/Strategy.cpp"
				 "include/Variations/CopyVariation.h"
				 "src/Variations/CrossoverVariation.cpp"
				 "src/Variations/SingleNucleotideVariation.cpp"
				 "src/Variations/VariationBase.cpp")

add_library(DEvA ${DEvA_SOURCES})
target_link_libraries(DEvA PRIVATE DEvA_version)
target_include_directories(DEvA SYSTEM PUBLIC "include")
target_include_directories(DEvA SYSTEM PUBLIC "include/Strategies")
target_include_directories(DEvA SYSTEM PUBLIC "include/Variations")
