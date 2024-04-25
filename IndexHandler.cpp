#include "IndexHandler.h"
#include "DSAvl_tree_ValuesMap.h"
#include "DSAvl_tree.h"
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