#include "QueryEngine.h"
#include "IndexHandler.h"
#include "porter2_stemmer/porter2_stemmer.h"

#include <iostream>
#include <sstream>
#include <map>
using namespace std;

QueryEngine::QueryEngine(IndexHandler& theIndex){
    index = &theIndex;
    theStopWords = new StopWords();
}

QueryEngine::~QueryEngine(){
    delete theStopWords;
}

string QueryEngine::search(string theQuery){
    // string stream for the Query
    istringstream stringStream(theQuery);

    // variables
    map<size_t, string> searchResult;
    set<string>* wordsToPass = new set<string>();
    string personToPass = "";
    string orgToPass = "";
    string evalString;

    wordsCheck(stringStream, evalString, wordsToPass);

    // if it reached the endbit of the stringStream, that means that "PERSON:" and "ORG:" were not typed, therefore:
    if(stringStream.eof()){
        // send to IndexHandler to get search result in map variable created earlier
        
        
        // REMEMBER TO UNCOMMENT THIS FOR CHRIST SAKE
        // searchResult = index->searchIndex(wordsToPass);

        // delete wordsToPass for memory management
        delete wordsToPass;

        // return the searchResults map
        // return searchResult;
        return everythingAsString(wordsToPass);
    }
    
    // either has to be "PERSON:" or "ORG:"; so, if statement only checks for those two.
    if(evalString.find("PERSON:") != string::npos){
        // "PERSON:" was typed after the words
        personToPass = personCheck(stringStream, evalString);
        if(stringStream.eof()){
            delete wordsToPass;
            // return searchResult;
            return everythingAsString(wordsToPass, personToPass, orgToPass);
        }
        orgToPass = orgCheck(stringStream, evalString);
        delete wordsToPass;
        // return searchResult;
        return everythingAsString(wordsToPass, personToPass, orgToPass);

    }
    else{
        // if the function has gotten to this point, that means "ORG:" was typed after the words.
        // "PERSON:" was typed after the words
        orgToPass = orgCheck(stringStream, evalString);
        if(stringStream.eof()){
            delete wordsToPass;
            // return searchResult;
            return everythingAsString(wordsToPass, personToPass, orgToPass);
        }
        personToPass = personCheck(stringStream, evalString);
        delete wordsToPass;
        // return searchResult;
        return everythingAsString(wordsToPass, personToPass, orgToPass);

    }

}

string QueryEngine::everythingAsString(set<string>* wordsToPass, string personToPass, string orgToPass){
    string allInfoAsString = "Words recieved in query: {";
    for(const auto& word: *wordsToPass){
        allInfoAsString += word + ", ";
    }

    // gets rid of the last ", "
    if(!wordsToPass->empty()){
        allInfoAsString.pop_back();
        allInfoAsString.pop_back();
    }

    allInfoAsString += "}, person: {" + personToPass + "} org: {" + orgToPass + "}";

    return allInfoAsString;
}

void QueryEngine::wordsCheck(istringstream& stringStream, string& evalString, set<string>* wordsToPass){
    // first word to evalString
    stringStream >> evalString;
    
    // if evalString doesn't have "PERSON:" AND "ORG:" AND string stream hasn't reached its endbit 
    while((evalString.find("PERSON:") == string::npos) && (evalString.find("ORG:") == string::npos) && !stringStream.eof()){
        if(evalString.size() < 2){
            stringStream >> evalString;
            continue;
        }
        // do the same process that was done to generate word trees in DocParser for evalString
        Porter2Stemmer::trim(evalString);
        Porter2Stemmer::stem(evalString);
        
        if(theStopWords->stopWordsTree->contains(evalString)){
            stringStream >> evalString;
            continue;
        }
        wordsToPass->insert(evalString);

        // go on to next value in stringStream
        stringStream >> evalString;
    }
}

string QueryEngine::personCheck(istringstream& stringStream, string& evalString){
    // this means that PERSON: was found
    string thePerson = "";
    // checks to see if the first word after is glued or spaced to ":"
    if(evalString.length() == 7){
        // spaced case
        stringStream >> evalString;
    }
    else{
        // glued case
        evalString = evalString.substr(7);
    }

    // assumes only one person can be searched for and not multiple people
    while((!stringStream.eof()) && (evalString.find("ORG:") == string::npos)){
        thePerson += evalString;
        stringStream >> evalString;
    }
    return thePerson;
}

string QueryEngine::orgCheck(istringstream& stringStream, string& evalString){
    // this means that ORG: was found
    string theOrg = "";
    // checks to see if the first word after is glued or spaced to ":"
    if(evalString.length() == 4){
        // spaced case
        stringStream >> evalString;
    }
    else{
        // glued case
        evalString = evalString.substr(4);
    }

    // assumes only one org can be searched for and not multiple orgs
    while((!stringStream.eof()) && (evalString.find("ORG:") == string::npos)){
        theOrg += evalString;
        stringStream >> evalString;
    }
    return theOrg;
}