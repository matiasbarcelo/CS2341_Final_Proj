#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "IndexHandler.h"
#include "DocParser.h"
#include "QueryEngine.h"
#include <vector>
#include <utility>

using namespace std;

class SearchEngine{
    private:
        IndexHandler* index;
        DocParser* theDocParser;
        QueryEngine* theQueryEngine;
        vector<pair<size_t, string>> searchResult;
        string fileDir;
        int resultsStart;
        int resultsLim;
    
    public:
        SearchEngine(string fileDirectory);
        ~SearchEngine();
        void superSearch(string theQuery);
        void displayResults();
        void displayText(string theNum);
        void nextPage();
        void previousPage();
        bool hasSearchResults();
};

#endif