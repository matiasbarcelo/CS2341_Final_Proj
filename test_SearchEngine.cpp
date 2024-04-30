#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "SearchEngine.h"
#include <iostream>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that SearchEngine works", "[SearchEngine]"){
    SearchEngine theSearchEngine = SearchEngine("sample_data");

    theSearchEngine.superSearch("fiscal");
    theSearchEngine.displayResults();

    theSearchEngine.displayText("0");

    theSearchEngine.superSearch("ORG: reuters");
    theSearchEngine.displayResults();

    theSearchEngine.displayText("0");
    theSearchEngine.displayText("1");

    theSearchEngine.superSearch("PERSON:JEROME POWELL");
    theSearchEngine.displayResults();

    // theSearchEngine.superSearch("test");
    // theSearchEngine.displayResults();
}