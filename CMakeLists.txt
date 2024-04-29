cmake_minimum_required(VERSION 3.15)
project(FinalProjExamples)

# filesystem.h require C++17
set(CMAKE_CXX_STANDARD 17)

file(COPY sample_data/ DESTINATION sample_data/)

# show compiler output and enable warnings
set(CMAKE_VERBOSE_MAKEFILE ON)
add_compile_options(-Wall -Wextra -pedantic)

# testing here
include(CTest)
add_executable(test_AvlTree test_DSAvl_tree.cpp)
add_test(NAME TestAvlTree COMMAND test_AvlTree)
add_executable(test_AvlTree_ValuesMap test_DSAvl_tree_ValuesMap.cpp)
add_test(NAME TestAvlTree_ValuesMap COMMAND test_AvlTree_ValuesMap)
add_executable(test_DocParser test_DocParser.cpp DocParser.cpp IndexHandler.cpp porter2_stemmer/porter2_stemmer.cpp)
add_test(NAME TestDocParser COMMAND test_DocParser)
add_executable(test_QueryEngine test_QueryEngine.cpp QueryEngine.cpp IndexHandler.cpp porter2_stemmer/porter2_stemmer.cpp)
add_test(NAME TestQueryEngine COMMAND test_QueryEngine)
add_executable(test_SearchEngine test_SearchEngine.cpp SearchEngine.cpp QueryEngine.cpp IndexHandler.cpp DocParser.cpp porter2_stemmer/porter2_stemmer.cpp)
add_test(NAME TestSearchEngine COMMAND test_SearchEngine)


add_executable(rapidJSONExample rapidJSONExample.cpp)
add_executable(supersearch main.cpp)

# this makes sure we also link rapidjson
target_include_directories(rapidJSONExample PRIVATE rapidjson/)

set(CMAKE_VERBOSE_MAKEFILE OFF)