#include "DocParser.h"
#include "DSAvl_tree.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <fstream>

#include "porter2_stemmer/porter2_stemmer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

using namespace std;

DocParser::DocParser(string theFileDir, IndexHandler& theIndex){
    index = &theIndex;
    fileDir = theFileDir;
    setupStopWords();
    parse();
}

void DocParser::setupStopWords(){
    stopwords = new DSAvl_tree<string, string>();

    // assumes that stopwords.csv is already
    ifstream theFile;
    theFile.open("../stopwords.csv");
    
    // if the file isn't there throw an error
    if(!theFile.is_open()){
        throw runtime_error("stopwords.csv not in working dir");
    }

    // defined outside loop so var only instantiated once
    string currentWord;

    // goes through file to find all words and adds it to stopwords avl tree
    while(!theFile.eof()){
        getline(theFile, currentWord, ',');
        stopwords->insert(currentWord);
    }

    // closes the file
    theFile.close();
}

DocParser::~DocParser(){
    delete stopwords;
}

void DocParser::printStopWords(){
    stopwords->prettyPrintTree();
}

string DocParser::getStopWordsAsString(){
    return stopwords->getKeysAsString();
}

void DocParser::parse(){
    parseWords();
    parsePeople();
    parseOrgs();
}

void DocParser::parseWords(){
    auto dir = filesystem::recursive_directory_iterator(fileDir);

    for(const auto& file : dir){
        filesystem::path filePath = file.path();
        string fileString = filePath.string();
        ifstream input(fileString);
        cout << fileString << endl;
        
        if(!input.is_open()){
            cout << "Hasn't terminated here yet" << endl;
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

            if(stopwords->contains(theWord) || theWord.size() < 2){
                continue;
            }

            index->addWord(theWord, fileString);
        }
    }
}

void DocParser::parsePeople(){
    auto dir = filesystem::recursive_directory_iterator(fileDir);

    for(const auto& file:dir){
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