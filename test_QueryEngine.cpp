#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "QueryEngine.h"
#include "IndexHandler.h"
#include <iostream>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that DocParser works", "[DocParser]"){
    IndexHandler indexExample = IndexHandler();
    QueryEngine queryEngineExample = QueryEngine(indexExample);

    cout << queryEngineExample.search("this is a fucking test PERSON:PATRICE O'NEAL ORG:Opie and Anthony") << endl;
    cout << queryEngineExample.search("this is a fucking test PERSON: ANTHONY CUMIA ORG: Opie and Anthony") << endl;
    cout << queryEngineExample.search("this is a fucking test ORG: Congress PERSON:MY SENATOR") << endl;

}