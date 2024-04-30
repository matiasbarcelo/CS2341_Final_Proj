#include "SearchEngine.h"
#include "IndexHandler.h"
#include "DocParser.h"
#include "QueryEngine.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include <vector>
#include <utility>
#include <iostream>
#include <fstream>

using namespace rapidjson;
using namespace std;

SearchEngine::SearchEngine(bool hasPersistance, string fileDirectory){
    fileDir = fileDirectory;
    index = new IndexHandler();
    if(hasPersistance){
        theDocParser = new DocParser(*index);
    }
    else{
        theDocParser = new DocParser(*index, false, fileDirectory);
        index -> persistTrees();
    }
    theQueryEngine = new QueryEngine(*index);
    resultsStart = 0;
    resultsLim = 14;
}

SearchEngine::~SearchEngine(){
    delete index;
    delete theDocParser;
    delete theQueryEngine;
}

void SearchEngine::superSearch(string theQuery){
    searchResult = theQueryEngine->superSearch(theQuery);
}

void SearchEngine::displayResults(){
    if(searchResult.empty()){
        cout << "No results!" << endl;
        return;
    }

    int tempResultLim = resultsLim;
    if(searchResult.size() - 1 < resultsLim){
        resultsLim = searchResult.size() - 1;
    }

    
    cout << "Number of results: " << to_string(searchResult.size()) << endl << endl;

    for(int i = resultsStart; i <= resultsLim; i++){
        ifstream input(searchResult.at(i).second);
        IStreamWrapper isw(input);
        Document d;
        d.ParseStream(isw);
        // need article title, publciation, and date published
        // in the json file that should be "title", "site", and "published"
        cout << "Result #" << to_string(i) << ": " << endl;
        cout << "SuperSearch Score: " << to_string(searchResult.at(i).first) << endl;
        
        cout << "Title: " << d["title"].GetString() << endl;
        cout << "Publication: " << d["thread"]["site_full"].GetString() << endl;
        cout << "Date published: " << d["published"].GetString() << endl << endl;
    }
    resultsLim = tempResultLim;
}

void SearchEngine::displayText(string theNum){
    int theNumAsInt = stoi(theNum);
    ifstream input(searchResult.at(theNumAsInt).second);
    IStreamWrapper isw(input);
    Document d;
    d.ParseStream(isw);
    cout << endl << d["text"].GetString() << endl << endl;
}

bool SearchEngine::hasSearchResults(){
    return !searchResult.empty();
}

void SearchEngine::next(){
    if(searchResult.size() < resultsLim + 1){
        cout << "Cannot display more results!" << endl;
        return;
    }
    resultsStart = resultsLim + 1;
    resultsLim = resultsLim + 15;
}

void SearchEngine::prev(){
    if(resultsStart - 15 < 0){
        cout << "Cannot display less results!" << endl;
        return;
    }
    resultsLim = (resultsStart + 14) - 15;
    resultsStart = resultsStart - 15;
}

void SearchEngine::showCounts(){
    cout << endl << "Num of words in tree: " << to_string(index->getWordCount()) << endl;
    cout << "Num of people in tree: " << to_string(index->getPeopleCount()) << endl;
    cout << "Num of orgs in tree: " << to_string(index->getOrgsCount()) << endl << endl;
}