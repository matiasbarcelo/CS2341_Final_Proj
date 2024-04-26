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

    cout << queryEngineExample.search("Red Robbin be robbing mad robbins PERSON:PATRICE O'NEAL ORG:Opie and Anthony") << endl;
    cout << queryEngineExample.search("this is a FREAKING test PERSON: ANTHONY CUMIA ORG: Opie and Anthony") << endl;
    cout << queryEngineExample.search("this is a freaking test I SWEAR ORG: Congress PERSON:MY SENATOR") << endl;
    cout << queryEngineExample.search("this time just some words") << endl;
    cout << queryEngineExample.search("PERSON:MATIAS BARCELO ORG: SMU") << endl;
    cout << queryEngineExample.search("PERSON:MATIAS BARCELo ") << endl;
    cout << queryEngineExample.search(" ORG: SMU") << endl;
    cout << queryEngineExample.search("ORG: SMU PERSON:MATIAS BARCELO") << endl;


}