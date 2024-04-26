#include "DocParser.h"
#include "DSAvl_tree.h"
#include <fstream>
#include <sstream>
#include <filesystem>

#include "porter2_stemmer/porter2_stemmer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "stopwords/StopWords.h"

using namespace std;

DocParser::DocParser(string theFileDir, IndexHandler& theIndex){
    index = &theIndex;
    fileDir = theFileDir;
    theStopWords = new StopWords();
    parse();
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