#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "QueryEngine.h"
#include "IndexHandler.h"
#include <iostream>
#include <vector>
#include <utility>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that QueryEngine works", "[QueryEngine]"){
    IndexHandler indexExample = IndexHandler();
    QueryEngine queryEngineExample = QueryEngine(indexExample);

    cout << queryEngineExample.stringSearch("Red Robbin be robbing mad robbins PERSON:PATRICE O'NEAL ORG:Opie and Anthony") << endl;
    cout << queryEngineExample.stringSearch("this is a FREAKING test PERSON: ANTHONY CUMIA ORG: Opie and Anthony") << endl;
    cout << queryEngineExample.stringSearch("this is a freaking test I SWEAR ORG: Congress PERSON:MY SENATOR") << endl;
    cout << queryEngineExample.stringSearch("this time just some words") << endl;
    cout << queryEngineExample.stringSearch("PERSON:MATIAS BARCELO ORG: SMU") << endl;
    cout << queryEngineExample.stringSearch("PERSON:MATIAS BARCELo ") << endl;
    cout << queryEngineExample.stringSearch(" ORG: SMU") << endl;
    cout << queryEngineExample.stringSearch("ORG: SMU PERSON:MATIAS BARCELO") << endl;
    
}