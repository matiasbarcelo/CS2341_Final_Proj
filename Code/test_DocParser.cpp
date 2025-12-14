#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "DocParser.h"
#include "IndexHandler.h"
#include <fstream>
#include <iostream>
using namespace std;


/**
 * most of the implementation is directly from the notes, which are right by default, so I am only testing for the new stuff I added.
 * I added getKeys(), getValues(), and insertValue()
*/

TEST_CASE("Checks that DocParser works", "[DocParser]"){
    IndexHandler indexExample = IndexHandler();
    string fileDir = "sample_data";
    DocParser parser = DocParser(indexExample, false, fileDir);
    string stopwordsString = parser.theStopWords->getStopWordsAsString();
    
    cout << stopwordsString << endl << endl;

    cout << indexExample.wordsTreeAsString() << endl << endl;

    cout << indexExample.peopleTreeAsString() << endl << endl;

    cout << indexExample.orgsTreeAsString() << endl << endl;
    
    IndexHandler indexExample2 = IndexHandler();
    DocParser parser2 = DocParser(indexExample2);

    
    cout << indexExample2.wordsTreeAsString() << endl << endl;

    cout << indexExample2.peopleTreeAsString() << endl << endl;

    cout << indexExample2.orgsTreeAsString() << endl << endl;


}