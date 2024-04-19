#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "DSAvl_tree.h"
#include <iostream>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that DSAvl_tree works", "[DSAvl_tree]"){
    
    // setting up the tree with some test values
    DSAvl_tree<string, int> testTree;
    string test1 = "test1";
    string test2 = "test2";
    string test3 = "test3";
    
    testTree.insert(test1);
    testTree.insert(test2);
    testTree.insert(test3);

    testTree.prettyPrintTree();

    // tests getKeys(), which uses LNR to return a vector
    vector<string> keysVector = testTree.getKeys();
    
    string theKeysAsStr = "";

    for(int i = 0; i < keysVector.size(); i++){
        if(i == keysVector.size() - 1){
            theKeysAsStr += keysVector.at(i);
            break;
        }
        theKeysAsStr += keysVector.at(i) + ", ";
    }

    REQUIRE(theKeysAsStr == "test1, test2, test3");
    
    // tests insertValue() and getValues()
    int value1 = 1;
    int value2 = 2;

    testTree.insertValue(test1, value1);
    testTree.insertValue(test1, value2);

    vector<int> valueVector = testTree.getValues(test1);

    REQUIRE(valueVector.at(0) == 1);
    REQUIRE(valueVector.at(1) == 2);
}