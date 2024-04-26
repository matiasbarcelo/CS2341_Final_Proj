#ifndef DOCPARSER_H
#define DOCPARSER_H
#include "DSAvl_tree.h"
#include "IndexHandler.h"
#include "stopwords/StopWords.h"
using namespace std;

class DocParser{
    private:
        IndexHandler* index;
        string fileDir;
    public:
        StopWords* theStopWords;
        DocParser(string theFileDir, IndexHandler& theIndex);
        ~DocParser();
        void setupStopWords();
        void parseStopWordsCSV();
        void makePerStopWords();
        void decodePerStopWords();
        void printPrettyTreeStopWords();
        string getStopWordsAsString();
        void parse();
        void parseWords();
        void parsePeople();
        void parseOrgs();
};

#endif