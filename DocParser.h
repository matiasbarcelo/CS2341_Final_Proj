#ifndef DOCPARSER_H
#define DOCPARSER_H
#include "DSAvl_tree.h"
#include "IndexHandler.h"
using namespace std;

class DocParser{
    private:
        DSAvl_tree<string, string> stopwords;
        IndexHandler* index;
    public:
        DocParser();
        
};

#endif