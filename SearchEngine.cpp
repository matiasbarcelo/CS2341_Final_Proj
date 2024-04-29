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

SearchEngine::SearchEngine(string fileDirectory){
    fileDir = fileDirectory;
    index = new IndexHandler();
    theDocParser = new DocParser(*index);
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

    for(int i = resultsStart; i <= resultsLim; i++){
        ifstream input(searchResult.at(i).second);
        IStreamWrapper isw(input);
        Document d;
        d.ParseStream(isw);
        // need article title, publciation, and date published
        // in the json file that should be "title", "site", and "published"
        cout << to_string(i) << ": " << endl;
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
    cout << d["text"].GetString() << endl << endl;
}

bool SearchEngine::hasSearchResults(){
    return !searchResult.empty();
}