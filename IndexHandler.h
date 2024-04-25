#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H
#include <iostream>
#include "DSAvl_tree.h"
#include "DSAvl_tree_ValuesMap.h"
#include <vector>
using namespace std;

class IndexHandler{
    private:
        DSAvl_tree_ValuesMap<string, string>* words;
        DSAvl_tree<string, string>* people;
        DSAvl_tree<string, string>* orgs;

    public:
        IndexHandler();
        IndexHandler(string percistanceFileDir);
        ~IndexHandler();
        void addWord(string theWord, string file_id);
        void addPerson(string person, string file_id);
        void addOrg(string org, string file_id);
        string wordsTreeAsString();
        string wordsKeysAsString();
        string peopleTreeAsString();
        string orgsTreeAsString();

};

#endif