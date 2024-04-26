#include "IndexHandler.h"
#include "DSAvl_tree_ValuesMap.h"
#include "DSAvl_tree.h"
#include <vector>
#include <set>
using namespace std;

IndexHandler::IndexHandler(){
    words = new DSAvl_tree_ValuesMap<string, string>();
    people = new DSAvl_tree<string,string>();
    orgs = new DSAvl_tree<string, string>();
}

IndexHandler::~IndexHandler(){
    delete words;
    delete people;
    delete orgs;
}

void IndexHandler::addWord(string theWord, string file_id){
    words->insertValue(theWord, file_id);
}

void IndexHandler::addPerson(string person, string file_id){
    people->insertValue(person, file_id);
}

void IndexHandler::addOrg(string org, string file_id){
    orgs->insertValue(org, file_id);
}

bool IndexHandler::wordsIsEmpty(){
    return words->isEmpty();
}

bool IndexHandler::peopleIsEmpty(){
    return people->isEmpty();
}

bool IndexHandler::orgsIsEmpty(){
    return orgs->isEmpty();
}

string IndexHandler::wordsTreeAsString(){
    return words->getKeysAndValuesMapAsString();
}

string IndexHandler::wordsKeysAsString(){
    return words->getKeysAsString();
}

string IndexHandler::orgsTreeAsString(){
    return orgs->getKeysAndValuesAsString();
}

string IndexHandler::peopleTreeAsString(){
    return people->getKeysAndValuesAsString();
}

// map<size_t, string> IndexHandler::searchIndex(set<string>* wordsSet, string person = "", string org = ""){
    
//     vector<set<string>> vectorOfWordSets;
    
//     for(const auto& word: *wordsSet){
//         vectorOfWordSets.push_back(words->getValuesAsSet(word));
//     }

//     set<string> personSet;
//     if(person != ""){
//         personSet = people->getValuesAsSet(person);
//     }

//     set<string> orgSet;
//     if(org != ""){
//         orgSet = orgs->getValuesAsSet(org);
//     }

// }
