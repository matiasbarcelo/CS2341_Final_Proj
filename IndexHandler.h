#ifndef INDEXHANDLER_H
#define INDEXHANDLER_H
#include <iostream>
#include "DSAvl_tree.h"
#include "DSAvl_tree_ValuesMap.h"
#include <map>
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
        bool wordsIsEmpty();
        bool peopleIsEmpty();
        bool orgsIsEmpty();
        string wordsTreeAsString();
        string wordsKeysAsString();
        string peopleTreeAsString();
        string orgsTreeAsString();

        // map<size_t, string> searchIndex(set<string>* wordsSet, string person = "", string org = "");
};

#endif