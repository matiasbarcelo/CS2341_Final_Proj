#ifndef QUERYENGINE_H
#define QUERYENGINE_H

#include "IndexHandler.h"
#include "stopwords/StopWords.h"
#include <set>

class QueryEngine{
    private:
        IndexHandler* index;
        StopWords* theStopWords;
    public:
        QueryEngine(IndexHandler& theIndex);
        ~QueryEngine();
        void wordsCheck(istringstream& stringStream, string& evalString, set<string>& wordsToPass);
        string personCheck(istringstream& stringStream, string& evalString);
        string orgCheck(istringstream& stringStream, string& evalString);
        string everythingAsString(set<string>& wordsToPass, string personToPass = "", string orgToPass = "");
        string search(string theQuery);
        void toLowerCase(string& person, string& org);

};

#endif