#include "DocParser.h"
#include "DSAvl_tree.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

#include "porter2_stemmer/porter2_stemmer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "stopwords/StopWords.h"

using namespace std;

DocParser::DocParser(IndexHandler& theIndex, bool hasPersistenceFiles, string theFileDir){
    index = &theIndex;
    fileDir = theFileDir;
    theStopWords = new StopWords();
    persistenceFiles = hasPersistenceFiles;
    if(!hasPersistenceFiles){
        parse();
    }
    else{
        parsePersistenceFiles();
    }

}

DocParser::~DocParser(){
    delete theStopWords;
}

void DocParser::parse(){
    if(index->wordsIsEmpty())
        parseWords();
    if(index->peopleIsEmpty())
        parsePeople();
    if(index->orgsIsEmpty())
        parseOrgs();
}

void DocParser::parseWords(){
    auto dir = filesystem::recursive_directory_iterator(fileDir);

    for(const auto& file : dir){
        if(filesystem::is_directory(file)){
            continue;
        }

        filesystem::path filePath = file.path();
        string fileString = filePath.string();
        ifstream input(fileString);

        if(!input.is_open()){
            throw runtime_error("Could not open" + fileString + " in docparser parse function");
        }

        rapidjson::IStreamWrapper isw(input);
        rapidjson::Document d;
        d.ParseStream(isw);

        string theWords = d["text"].GetString();

        istringstream stringStream(theWords);

        string theWord;
        
        while(stringStream >> theWord){

            // trims and stems the word
            Porter2Stemmer::trim(theWord);
            Porter2Stemmer::stem(theWord);

            if(theStopWords->stopWordsTree->contains(theWord) || theWord.size() < 2){
                continue;
            }

            index->addWord(theWord, fileString);
        }
    }
}

void DocParser::parsePeople(){
    auto dir = filesystem::recursive_directory_iterator(fileDir);

    for(const auto& file:dir){
        if(filesystem::is_directory(file)){
            continue;
        }
        
        filesystem::path filePath = file.path();
        string fileString = filePath.string();
        ifstream input(fileString);
        
        if(!input.is_open()){
            throw runtime_error("Could not open" + fileString + " in docparser parse function");
        }

        rapidjson::IStreamWrapper isw(input);
        rapidjson::Document d;
        d.ParseStream(isw);
        auto thePeople = d["entities"]["persons"].GetArray();
        
        for(const auto& person: thePeople){
            string personString = person["name"].GetString();
            index->addPerson(personString, fileString);
        }

    }
}


void DocParser::parseOrgs(){
    auto dir = filesystem::recursive_directory_iterator(fileDir);

    for(const auto& file:dir){
        if(filesystem::is_directory(file)){
            continue;
        }
        
        filesystem::path filePath = file.path();
        string fileString = filePath.string();
        ifstream input(fileString);
        
        if(!input.is_open()){
            throw runtime_error("Could not open" + fileString + " in docparser parse function");
        }

        rapidjson::IStreamWrapper isw(input);
        rapidjson::Document d;
        d.ParseStream(isw);
        auto theOrgs = d["entities"]["organizations"].GetArray();
        
        for(const auto& org: theOrgs){
            string orgString = org["name"].GetString();
            index->addOrg(orgString, fileString);
        }
    }
}

void DocParser::parsePersistenceFiles(){
    
    // currentLine for all the files because I can't redeclare it
    string currentLine;

    // this does persistance for the words tree
    fstream file;
    file.open("../persistance_files/word_tree.txt");
    if(!file.is_open()){
        throw runtime_error("Could not open word_tree.txt file");
    }
    
    while(getline(file, currentLine)){
        // creates a variable for the word
        string theWord;
        // the map that is going to be passed into Avltree with map value
        map<string, size_t> valuesMap;

        // finds the word
        size_t colonPos = currentLine.find(":");
        theWord = currentLine.substr(0, colonPos);
        
        // finds the values
        size_t curlyBracketStartPos = currentLine.find("{");
        size_t curlyBracketEndPos = currentLine.find("}");
        string theValues = currentLine.substr(curlyBracketStartPos + 1, curlyBracketEndPos - curlyBracketStartPos - 1);

        // creates a string stream for the values
        istringstream stringStream(theValues);
        string theValuePair;
        while(getline(stringStream, theValuePair, ',')){
            size_t colonPos2 = theValuePair.find(":");
            string theDoc = theValuePair.substr(1, colonPos2 - 1);
            string theNum = theValuePair.substr(colonPos2 + 2);
            theNum.pop_back();
            size_t numAsNum = stoi(theNum);
            // this happens because there are spaces between the docs
            if(theDoc.at(0) == '('){
                theDoc.erase(theDoc.begin());
            }
            valuesMap[theDoc] = numAsNum;
        }
        index->addWordWithMap(theWord, valuesMap);
    }
    file.close();

    // this does persistance for people tree
    file.open("../persistance_files/people_tree.txt");
    if(!file.is_open()){
        throw runtime_error("Could not open people_tree.txt file");
    }

    while(getline(file, currentLine)){
        // creates a variable for the word
        string thePerson;

        // finds the word
        size_t colonPos = currentLine.find(":");
        thePerson = currentLine.substr(0, colonPos);
        
        // finds the values
        size_t curlyBracketStartPos = currentLine.find("{");
        size_t curlyBracketEndPos = currentLine.find("}");
        string theValues = currentLine.substr(curlyBracketStartPos + 1, curlyBracketEndPos - curlyBracketStartPos - 1);

        // creates a string stream for the values
        istringstream stringStream(theValues);
        string theValue;
        while(getline(stringStream, theValue, ',')){
            // this happens because there are spaces between the docs
            if(theValue.at(0) == ' '){
                theValue.erase(theValue.begin());
            }
            index->addPerson(thePerson, theValue);
        }
    }
    file.close();

    // this does persistance for orgs tree
    file.open("../persistance_files/orgs_tree.txt");
    if(!file.is_open()){
        throw runtime_error("Could not open people_tree.txt file");
    }
    while(getline(file, currentLine)){
        // creates a variable for the word
        string theOrg;

        // finds the word
        size_t colonPos = currentLine.find(":");
        theOrg = currentLine.substr(0, colonPos);
        
        // finds the values
        size_t curlyBracketStartPos = currentLine.find("{");
        size_t curlyBracketEndPos = currentLine.find("}");
        string theValues = currentLine.substr(curlyBracketStartPos + 1, curlyBracketEndPos - curlyBracketStartPos - 1);

        // creates a string stream for the values
        istringstream stringStream(theValues);
        string theValue;
        while(getline(stringStream, theValue, ',')){
            if(theValue.at(0) == ' '){
                theValue.erase(theValue.begin());
            }
            index->addOrg(theOrg, theValue);
        }
    }
    file.close();

}