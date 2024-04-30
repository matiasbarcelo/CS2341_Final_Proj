#include <iostream>
#include "SearchEngine.h"
#include <filesystem>
// #include <chronos>

using namespace std;


bool checkPersistence(){
    // these are the paths of the persistence files (remember that the executable is in build dir thus "../" for file navigation)
    filesystem::path wordsFile("../persistance_files/word_tree.txt");
    filesystem::path peopleFile("../persistance_files/people_tree.txt");
    filesystem::path orgsFile("../persistance_files/orgs_tree.txt");


    // if any of these aren't in their proper spot return false, otherwise return true
    if((!filesystem::exists(wordsFile)) || (!filesystem::exists(peopleFile)) || (!filesystem::exists(orgsFile))){
        return false;
    }
    return true;
}

string getFileDir(){
    string fileDir;
    cout << "Persistence files not found in persistance_files dir." << endl;
    cout << "Please type in the directory containing all the document files (keep in mind you are in the build folder): ";
    getline(cin, fileDir);

    // this loop executes when the directory does not exist or is not a directory but rather a file
    while((!filesystem::exists(fileDir)) || (!filesystem::is_directory(fileDir))){
        // output when dir is not found to exist
        if(!filesystem::exists(fileDir)){
            cout << "Directory not found or does not exist. Please try again: ";
        }
        // output when not a dir
        else{
            cout << "Input recieved not a file directory. Please try again: ";
        }
        getline(cin, fileDir);
    }
    return fileDir;
}

int main()
{
    // persistence file detected or not detected
    bool hasPersistence = checkPersistence();
    // asks for dir if not detected, automatically creates persistance file
    SearchEngine* theSearchEngine;

    if(hasPersistence){
        cout << "Persistence files found in persistance_files dir" << endl;
        cout << "Please keep in mind that persistence is automatic. ";
        cout << "If you have a new dir of data please delete old persistence files and run supersearch again" << endl;
        theSearchEngine = new SearchEngine();
    }
    else{
        string fileDir = getFileDir();
        theSearchEngine = new SearchEngine(false, fileDir);
    }

    // shows some search engine stats
    theSearchEngine -> showCounts();

    // starts query
    string response = "";
    bool searchWentThrough = false;

    while(response != "q"){
        cout << "Please type in search: ";
        getline(cin, response);
        try{
            theSearchEngine->superSearch(response);
            theSearchEngine->displayResults();
            searchWentThrough = true;
            cout << "Type number of result to display article text (i.e. for the first article text type '0')." << 
                    " Press enter for new query. Type 'q' to quit." << endl;
            cout << ">";
            getline(cin, response);
            while((response != "") && (response != "q")){
                try{
                    // go to next page
                    if(response == "next"){
                        theSearchEngine->next();
                        theSearchEngine->displayResults();
                    }
                    // go to prev page
                    else if(response == "prev"){
                        theSearchEngine->prev();
                        theSearchEngine->displayResults();
                    }
                    // this means a command to display a text was chosen
                    else{
                        theSearchEngine->displayText(response);
                        cout << "Type number of result to display article text (i.e. for the first article text type '0'. To go to next or prev page type 'next' or 'prev')." << 
                        " Press enter for new query. Type 'q' to quit." << endl;
                    }
                    cout << ">";
                    getline(cin, response);
                }
                catch(const exception& e){
                    cout << "Num of result not recognized." << endl;
                    cout << "Type number of result to display article text (i.e. for the first article text type '0')." << 
                    " Press enter for new query. Type 'q' to quit." << endl;
                    cout << ">";
                    getline(cin, response);
                }
            }
        }
        catch(const exception& e){
            cout << "No results for that query" << endl;
        }
        
        // either read one of the options, next page, new query, or quit
        if(!searchWentThrough){
            cout << "Press enter for new query. Type 'q' to quit." << endl;
            cout << ">";
            getline(cin, response);
        }
    }
    delete theSearchEngine;
    return 0;
}