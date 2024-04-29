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
        bool persistenceFiles;
    public:
        StopWords* theStopWords;
        DocParser(IndexHandler& theIndex, bool hasPersistenceFiles=true, string theFileDir = "");
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
        void parsePersistenceFiles();
};

#endif