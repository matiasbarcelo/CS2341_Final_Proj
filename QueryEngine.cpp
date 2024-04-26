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
    set<string> wordsToPass;
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
        

        // return the searchResults map
        // return searchResult;
        return everythingAsString(wordsToPass);
    }
    
    // either has to be "PERSON:" or "ORG:"; so, if statement only checks for those two.
    if(evalString.find("PERSON:") != string::npos){
        // "PERSON:" was typed after the words
        personToPass = personCheck(stringStream, evalString);
        if(stringStream.eof()){
            
            // return searchResult;
            return everythingAsString(wordsToPass, personToPass, orgToPass);
        }
        orgToPass = orgCheck(stringStream, evalString);
        
        // return searchResult;
        return everythingAsString(wordsToPass, personToPass, orgToPass);

    }
    else{
        // if the function has gotten to this point, that means "ORG:" was typed after the words.
        // "PERSON:" was typed after the words
        orgToPass = orgCheck(stringStream, evalString);
        if(stringStream.eof()){
            
            // return searchResult;
            return everythingAsString(wordsToPass, personToPass, orgToPass);
        }
        personToPass = personCheck(stringStream, evalString);
        
        // return searchResult;
        return everythingAsString(wordsToPass, personToPass, orgToPass);

    }

}

string QueryEngine::everythingAsString(set<string>& wordsToPass, string personToPass, string orgToPass){
    string allInfoAsString = "Words recieved in query: {";
    for (const auto& word : wordsToPass){
        string modifiedWord = word;
        for (char& letter : modifiedWord){
            letter = tolower(letter);
        }
        allInfoAsString += modifiedWord + ", ";
    }

    // gets rid of the last ", "
    if(!wordsToPass.empty()){
        allInfoAsString.pop_back();
        allInfoAsString.pop_back();
    }

    toLowerCase(personToPass, orgToPass);

    allInfoAsString += "}, person: {" + personToPass + "} org: {" + orgToPass + "}";

    return allInfoAsString;
}

void QueryEngine::toLowerCase(string& personToPass, string& orgToPass){
    for(auto& letter: personToPass){
        letter = tolower(letter);
    }

    for(auto& letter: orgToPass){
        letter = tolower(letter);
    }
}


void QueryEngine::wordsCheck(istringstream& stringStream, string& evalString, set<string>& wordsToPass){
    
    while(stringStream >> evalString){
        // if "PERSON:" or "ORG:" are found. Break out of the function.
        if((evalString.find("PERSON:") != string::npos) || (evalString.find("ORG:") != string::npos)){
            break;
        }

        // do the same process that was done to generate word trees in DocParser for evalString
        // if the word is smaller than 2 chars, skip it
        if(evalString.size() < 2){
            continue;
        }

        // trim and stem
        Porter2Stemmer::trim(evalString);
        Porter2Stemmer::stem(evalString);
        
        // if the word is a stop word, skip it.
        if(theStopWords->stopWordsTree->contains(evalString)){
            continue;
        }

        // insert it in the wordsToPass set
        wordsToPass.insert(evalString);
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

    thePerson += evalString;

    // assumes only one person can be searched for and not multiple people
    while(stringStream >> evalString){
        if(evalString.find("ORG:") != string::npos){
            break;
        }

        thePerson += " " + evalString;
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

    theOrg += evalString;

    // assumes only one org can be searched for and not multiple orgs
    while(stringStream >> evalString){
        if(evalString.find("PERSON:") != string::npos){
            break;
        }

        theOrg += " " + evalString;
    }
    return theOrg;
}