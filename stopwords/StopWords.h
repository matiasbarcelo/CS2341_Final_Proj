#ifndef STOPWORDS_H
#define STOPWORDS_H

#include "../DSAvl_tree.h"
#include <filesystem>
#include <fstream>
#include <vector>
#include "../PathUtils.h"
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

        // try several candidate locations for the stopwords CSV
        vector<filesystem::path> candidates;
        candidates.push_back(filesystem::path("../stopwords/stopwords.csv"));
        candidates.push_back(filesystem::path("stopwords/stopwords.csv"));
        auto exeDir = util::getExecutableDir();
        if(!exeDir.empty()){
            candidates.push_back(exeDir / "stopwords" / "stopwords.csv");
        }

        ifstream theFile;
        bool opened = false;
        for(const auto &p : candidates){
            if(filesystem::exists(p)){
                theFile.open(p);
                if(theFile.is_open()){
                    opened = true;
                    break;
                }
            }
        }

        if(!opened){
            string msg = "stopwords.csv not found. Tried: ";
            for(const auto &p : candidates) msg += p.string() + " ";
            throw runtime_error(msg);
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