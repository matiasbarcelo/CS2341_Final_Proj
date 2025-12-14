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
# Porter2 stemmer handling: prefer local copy, otherwise FetchContent from GitHub
include(FetchContent)
set(PORTER2_GIT_URL "https://github.com/smassung/porter2_stemmer.git" CACHE STRING "Git URL for porter2_stemmer")
set(PORTER2_GIT_TAG "master" CACHE STRING "Git tag/branch for porter2_stemmer to fetch if not present locally")

if(EXISTS "${CMAKE_SOURCE_DIR}/porter2_stemmer/porter2_stemmer.cpp")
	add_library(porter2_stemmer STATIC porter2_stemmer/porter2_stemmer.cpp)
	target_include_directories(porter2_stemmer PUBLIC ${CMAKE_SOURCE_DIR}/porter2_stemmer)
else()
	FetchContent_Declare(porter2_stemmer_src GIT_REPOSITORY ${PORTER2_GIT_URL} GIT_TAG ${PORTER2_GIT_TAG})
	FetchContent_GetProperties(porter2_stemmer_src)
	if(NOT porter2_stemmer_src_POPULATED)
		FetchContent_Populate(porter2_stemmer_src)
	endif()
	add_library(porter2_stemmer STATIC ${porter2_stemmer_src_SOURCE_DIR}/porter2_stemmer.cpp)
	target_include_directories(porter2_stemmer PUBLIC ${porter2_stemmer_src_SOURCE_DIR})
endif()

add_executable(test_DocParser test_DocParser.cpp DocParser.cpp IndexHandler.cpp PathUtils.cpp)
target_link_libraries(test_DocParser PRIVATE porter2_stemmer)
add_test(NAME TestDocParser COMMAND test_DocParser)
add_executable(test_QueryEngine test_QueryEngine.cpp QueryEngine.cpp IndexHandler.cpp PathUtils.cpp)
target_link_libraries(test_QueryEngine PRIVATE porter2_stemmer)
add_test(NAME TestQueryEngine COMMAND test_QueryEngine)
add_executable(test_SearchEngine test_SearchEngine.cpp SearchEngine.cpp QueryEngine.cpp IndexHandler.cpp DocParser.cpp PathUtils.cpp)
target_link_libraries(test_SearchEngine PRIVATE porter2_stemmer)
add_test(NAME TestSearchEngine COMMAND test_SearchEngine)


add_executable(rapidJSONExample rapidJSONExample.cpp)
add_executable(supersearch main.cpp SearchEngine.cpp QueryEngine.cpp IndexHandler.cpp DocParser.cpp PathUtils.cpp)
target_link_libraries(supersearch PRIVATE porter2_stemmer)

# Installation rules to produce a redistributable package with CPack
install(TARGETS supersearch
	RUNTIME DESTINATION bin
)

# Install resource directories that the executable expects at runtime
install(DIRECTORY ${CMAKE_SOURCE_DIR}/stopwords DESTINATION . PATTERN "*.csv" PATTERN "*.txt" )
install(DIRECTORY ${CMAKE_SOURCE_DIR}/persistance_files DESTINATION .)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/sample_data DESTINATION .)

include(InstallRequiredSystemLibraries)
# Set CPACK license resource to an existing README (prefer repo root)
if(EXISTS "${CMAKE_SOURCE_DIR}/../README.md")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/../README.md")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/README.md")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/README.md")
endif()
set(CPACK_PACKAGE_NAME "supersearch")
set(CPACK_PACKAGE_VENDOR "CS2341_Final_Proj")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SuperSearch - simple search engine")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_GENERATOR "ZIP")
include(CPack)

# this makes sure we also link rapidjson
target_include_directories(rapidJSONExample PRIVATE rapidjson/)

set(CMAKE_VERBOSE_MAKEFILE OFF)