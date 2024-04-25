#ifndef DOCPARSER_H
#define DOCPARSER_H
#include "DSAvl_tree.h"
#include "IndexHandler.h"
using namespace std;

class DocParser{
    private:
        DSAvl_tree<string, string>* stopwords;
        IndexHandler* index;
        string fileDir;
    public:
        DocParser(string theFileDir, IndexHandler& theIndex);
        ~DocParser();
        void setupStopWords();
        void printStopWords();
        string getStopWordsAsString();
        void parse();
        void parseWords();
        void parsePeople();
        void parseOrgs();
};

#endif