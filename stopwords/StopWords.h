#ifndef STOPWORDS_H
#define STOPWORDS_H

#include "../DSAvl_tree.h"
#include <filesystem>
#include <fstream>
using namespace std;

class StopWords{
    public:
    DSAvl_tree<string,string>* stopWordsTree;
    StopWords(){
        setupstopWordsTree();
    };

    ~StopWords(){
        delete stopWordsTree;
    }
    void setupstopWordsTree(){
        filesystem::path pathToPersistantStopWordsFile = "stopwords_PER.txt";

        if(!filesystem::exists(pathToPersistantStopWordsFile)){
            parseStopWordsCSV();
            makePerstopWordsTree();
        }
        else{
            decodePerStopWords();
        }
    }

    void makePerstopWordsTree(){

    }

    void decodePerStopWords(){

    }

    void parseStopWordsCSV(){
        stopWordsTree = new DSAvl_tree<string, string>();

        // assumes that stopwords.csv is already
        ifstream theFile;
        theFile.open("../stopwords/stopwords.csv");
        
        // if the file isn't there throw an error
        if(!theFile.is_open()){
            throw runtime_error("stopwords.csv not in stopwords dir");
        }

        // defined outside loop so var only instantiated once
        string currentWord;

        // goes through file to find all words and adds it to stopwords avl tree
        while(!theFile.eof()){
            getline(theFile, currentWord, ',');
            stopWordsTree->insert(currentWord);
        }

        // closes the file
        theFile.close();
    }

    void printPrettyTreeStopWords(){
        stopWordsTree->prettyPrintTree();
    }

    string getStopWordsAsString(){
        return stopWordsTree->getKeysAsString();
    }
};

#endif