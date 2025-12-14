#include "IndexHandler.h"
#include "DSAvl_tree_ValuesMap.h"
#include "DSAvl_tree.h"
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <fstream>
#include "PathUtils.h"
// #include "rapidjson/istreamwrapper.h"
// #include "rapidjson/document.h"
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

void IndexHandler::addWordWithMap(string& theWord, map<string, size_t>& aMap){
    words->insertValuesWithGivenMap(theWord, aMap);
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

void IndexHandler::persistTrees(){
    ofstream fileHandler;

    // Try to write to persistance_files relative to CWD first, otherwise next to the executable
    vector<filesystem::path> candidatesBase;
    candidatesBase.push_back(filesystem::path("../persistance_files"));
    auto exeDir = util::getExecutableDir();
    if(!exeDir.empty()) candidatesBase.push_back(exeDir / "persistance_files");

    filesystem::path baseDir;
    for(const auto &b : candidatesBase){
        // try to create directory if missing
        if(!filesystem::exists(b)){
            try{ filesystem::create_directories(b); } catch(...){}
        }
        if(filesystem::exists(b)){
            baseDir = b;
            break;
        }
    }

    if(baseDir.empty()){
        throw runtime_error("Could not find or create persistance_files directory (tried ../persistance_files and exe-dir/persistance_files)");
    }

    fileHandler.open((baseDir / "word_tree.txt").string());
    fileHandler << words->getKeysAndValuesMapAsString(false);
    fileHandler.close();

    fileHandler.open((baseDir / "people_tree.txt").string());
    fileHandler << people->getKeysAndValuesAsString(false);
    fileHandler.close();

    fileHandler.open((baseDir / "orgs_tree.txt").string());
    fileHandler << orgs->getKeysAndValuesAsString(false);
    fileHandler.close();
}

vector<pair<size_t,string>> IndexHandler::searchIndex(set<string> wordsSet, string person, string org){
    
    // this is a vector which holds the document set for each word
    vector<set<string>> vectorContainingDocSetForEachWord;

    // gets the doc id sets for each words and puts it in the vector
    for(const auto& word: wordsSet){
        vectorContainingDocSetForEachWord.push_back(words->getValuesAsSet(word));
    }

    // gets the person's set of docs
    set<string> personSet;
    if(person != ""){
        try{
            personSet = people->getValuesAsSet(person);
        }
        catch(const exception& e){
        }
    }

    // gets the org's set of docs
    set<string> orgSet;
    if(org != ""){
        try{
            orgSet = orgs->getValuesAsSet(org);
        }
        catch(const exception& e){
        }
    }

    // figures out which docs contain all the words
    set<string> intersectDocSetForWords;

    // if there is more than one doc set in vector, figure out their intersects
    if(vectorContainingDocSetForEachWord.size() > 1){
        // the interesectionDocSetForWords is now the first item in vectorContainingDocSetForEachWord
        intersectDocSetForWords = vectorContainingDocSetForEachWord.at(0);

        // this is a temporary set so I can run the algorithm
        set<string> tempSet;

        // start at the second item in vectorContainingDocSetForEachWord
        for(int i = 1; i < vectorContainingDocSetForEachWord.size(); i++){
            // clear the tempSet for because otherwise it would add sets again and not get rid of sets that don't intersect
            tempSet.clear();

            // this is the algorithm what docs in the set intersect
            set_intersection(intersectDocSetForWords.begin(), intersectDocSetForWords.end(),
                            vectorContainingDocSetForEachWord.at(i).begin(), vectorContainingDocSetForEachWord.at(i).end(),
                            inserter(tempSet, tempSet.begin()));
            intersectDocSetForWords = tempSet;
        }
    }
    // if there is only one, that is the intersect
    else if(vectorContainingDocSetForEachWord.size() == 1){
        intersectDocSetForWords = vectorContainingDocSetForEachWord.at(0);
    }

    // otherwise intersectDocSetForWords is empty

    // make an intersect set for all the docs that interset optionally words, optionally person, and optionally org
    set<string> intersectAll = intersectDocSetForWords;

    // search result with words, person, and org
    if((!intersectDocSetForWords.empty()) && (!personSet.empty()) && (!orgSet.empty())){
        set<string> tempSet;
        set_intersection(intersectAll.begin(), intersectAll.end(),
                        personSet.begin(), personSet.end(),
                        inserter(tempSet, tempSet.begin()));
        intersectAll = tempSet;
        tempSet.clear();
        set_intersection(intersectAll.begin(), intersectAll.end(),
                        orgSet.begin(), orgSet.end(),
                        inserter(tempSet, tempSet.begin()));
        intersectAll = tempSet;
    }
    
    // search result with words and person
    else if((!intersectDocSetForWords.empty()) && (!personSet.empty())){
        set<string> tempSet;
        set_intersection(intersectAll.begin(), intersectAll.end(),
                        personSet.begin(), personSet.end(),
                        inserter(tempSet, tempSet.begin()));
        intersectAll = tempSet;
    }

    // search result with words and org
    else if((!intersectDocSetForWords.empty()) && (!orgSet.empty())){
        set<string> tempSet;
        set_intersection(intersectAll.begin(), intersectAll.end(),
                        orgSet.begin(), orgSet.end(),
                        inserter(tempSet, tempSet.begin()));
        intersectAll = tempSet;
    }
    // search result with person and org
    else if((!personSet.empty()) && (!orgSet.empty())){
        intersectAll = personSet;
        set<string> tempSet;
        set_intersection(intersectAll.begin(), intersectAll.end(),
                        orgSet.begin(), orgSet.end(),
                        inserter(tempSet, tempSet.begin()));
        intersectAll = tempSet;
    }
    // search result with words only
    else if((!intersectDocSetForWords.empty())){
        // intersectAll already equals intersectWords so win by doing nothing
    }
    // search result with person only
    else if((!personSet.empty())){
        intersectAll = personSet;
    }
    // search result with org only
    else if((!orgSet.empty())){
        intersectAll = orgSet;
    }

    // now we have all the intersecting docs for words, person, and org
    /**
     * create a vector with pair values (because if I did this with maps, there could only be one value assigned to each number. This is a workaround)
     * size_t value which determines search priority on the left and document id on the right
    */
    vector<pair<size_t, string>> pairsOfValuesWithDocs;

    // create a doc value
    size_t docVal = 0;

    // if there are no words in the query, that means there is no search priority value, and only God (or a better computer scientist than I) knows how the priority is determined
    
    // if there are docs that meet all the criteria
    // if(!intersectAll.empty()){
        // if there were docs for the words that met the criteria
        if(!intersectDocSetForWords.empty()){
            // for each doc in the intersectAll set            
            for(const auto& doc: intersectAll){
                // for each word in the original wordsSet, which by definition needs to be in all the docs
                for(const auto& word: wordsSet){                    
                    docVal += words->getValueOfDocUsingKey(word, doc);
                }
                pair<size_t, string> thePair = make_pair(docVal, doc);
                pairsOfValuesWithDocs.push_back(thePair);
            }
            // use quicksort to sort the vector, greater<>() for descending order
            sort(pairsOfValuesWithDocs.begin(), pairsOfValuesWithDocs.end(), greater<>());
            return pairsOfValuesWithDocs;
        }
    // }
    else{
        for(const auto& doc: intersectAll){
            pair<size_t, string> thePair = make_pair(docVal, doc);
            pairsOfValuesWithDocs.push_back(thePair);
        }
        return pairsOfValuesWithDocs;
    }
}

size_t IndexHandler::getWordCount(){
    return words->getWordCount();
}

size_t IndexHandler::getPeopleCount(){
    return people->getCount();
}

size_t IndexHandler::getOrgsCount(){
    return orgs->getCount();
}