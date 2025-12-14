#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "DSAvl_tree_ValuesMap.h"
#include <iostream>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that DSAvl_tree_ValuesMap works", "[DSAvl_tree_ValuesMap]"){
    
    // setting up the tree with some test values
    DSAvl_tree_ValuesMap<string, string> testTree;
    string test1 = "test1";
    string test2 = "test2";
    string test3 = "test3";
    
    testTree.insert(test1);
    testTree.insert(test2);
    testTree.insert(test3);

    // tests to see if can add new key with value
    string test4 = "test4";
    string valueTest = "doc_1";
    testTree.insertValue(test4, valueTest);

    testTree.prettyPrintTree();

    // tests getKeysAsSet(), which uses LNR to return a set
    set<string> theKeysAsSet = testTree.getKeysAsSet();

    // tests to see that keys were found in the sest
    REQUIRE(theKeysAsSet.find(test1) != theKeysAsSet.end());
    REQUIRE(theKeysAsSet.find(test2) != theKeysAsSet.end());
    REQUIRE(theKeysAsSet.find(test3) != theKeysAsSet.end());

    // test to see if getKeysAsString() works
    string theKeysAsStr = testTree.getKeysAsString();
    cout << theKeysAsStr << endl;

    REQUIRE(theKeysAsStr == "test1, test2, test3, test4");
    
    // Does same thing as before but this time for values as map
    string value1 = "doc_1";
    string value2 = "doc_2";

    testTree.insertValue(test1, value1);
    testTree.insertValue(test1, value2);

    map<string, size_t> theValuesOfTest1KeyAsMap = testTree.getValuesAsMap(test1);

    REQUIRE(theValuesOfTest1KeyAsMap.find(value1) != theValuesOfTest1KeyAsMap.end());
    REQUIRE(theValuesOfTest1KeyAsMap.find(value2) != theValuesOfTest1KeyAsMap.end());

    string theValuesOfTest1KeyAsStr = testTree.getValuesMapAsString(test1);
    cout << theValuesOfTest1KeyAsStr << endl;

    REQUIRE(theValuesOfTest1KeyAsStr == "(doc_1: 1), (doc_2: 1)");

    testTree.insertValue(test1, value2);

    // checks to see if .getKeysAndValuesAsString() works
    string theKeysAndValuesAsStr = testTree.getKeysAndValuesMapAsString();
    cout << theKeysAndValuesAsStr << endl;

    REQUIRE(theKeysAndValuesAsStr == "test1: {(doc_1: 1), (doc_2: 2)}, test2: {}, test3: {}, test4: {(doc_1: 1)}");
}