#include "SearchEngine.h"
#include "IndexHandler.h"
#include "DocParser.h"
#include "QueryEngine.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include <vector>
#include <utility>
#include <iostream>
#include <fstream>
#include <chrono>
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <cctype>

using namespace rapidjson;
using namespace std;

namespace {
string jsonStringOrEmpty(const Document& d, const char* key) {
    if (!d.HasMember(key) || !d[key].IsString()) {
        return "";
    }
    return d[key].GetString();
}

string publicationFromDoc(const Document& d) {
    if (d.HasMember("thread") && d["thread"].IsObject()) {
        const auto& thread = d["thread"];
        if (thread.HasMember("site_full") && thread["site_full"].IsString()) {
            return thread["site_full"].GetString();
        }
        if (thread.HasMember("site") && thread["site"].IsString()) {
            return thread["site"].GetString();
        }
    }
    return "";
}
} // namespace

SearchEngine::SearchEngine(bool hasPersistance, string fileDirectory){
    fileDir = fileDirectory;
    index = new IndexHandler();
    if(hasPersistance){
        theDocParser = new DocParser(*index);
    }
    else{
        // Normalize to an absolute path so article paths stored in the index stay valid.
        if (!fileDirectory.empty()) {
            error_code ec;
            auto abs = filesystem::absolute(fileDirectory, ec);
            if (!ec) {
                fileDir = abs.lexically_normal().string();
            }
        }
        theDocParser = new DocParser(*index, false, fileDir);
        index -> persistTrees();
    }
    theQueryEngine = new QueryEngine(*index);
    resultsStart = 0;
    resultsLim = 14;
    persistence = hasPersistance;
}

SearchEngine::~SearchEngine(){
    delete index;
    delete theDocParser;
    delete theQueryEngine;
}

void SearchEngine::superSearch(string theQuery){
    searchResult = theQueryEngine->superSearch(theQuery);
}

void SearchEngine::displayResults(){
    if(searchResult.empty()){
        cout << "No results!" << endl;
        return;
    }

    int tempResultLim = resultsLim;
    if(searchResult.size() - 1 < resultsLim){
        resultsLim = searchResult.size() - 1;
    }

    
    cout << "Number of results: " << to_string(searchResult.size()) << endl << endl;

    for(int i = resultsStart; i <= resultsLim; i++){
        ifstream input(searchResult.at(i).second);
        IStreamWrapper isw(input);
        Document d;
        d.ParseStream(isw);
        // need article title, publciation, and date published
        // in the json file that should be "title", "site", and "published"
        cout << "Result #" << to_string(i) << ": " << endl;
        cout << "SuperSearch Score: " << to_string(searchResult.at(i).first) << endl;
        
        cout << "Title: " << d["title"].GetString() << endl;
        cout << "Publication: " << d["thread"]["site_full"].GetString() << endl;
        cout << "Date published: " << d["published"].GetString() << endl << endl;
    }
    resultsLim = tempResultLim;
}

void SearchEngine::displayText(string theNum){
    int theNumAsInt = stoi(theNum);
    ifstream input(searchResult.at(theNumAsInt).second);
    IStreamWrapper isw(input);
    Document d;
    d.ParseStream(isw);
    cout << endl << d["text"].GetString() << endl << endl;
}

bool SearchEngine::hasSearchResults(){
    return !searchResult.empty();
}

void SearchEngine::next(){
    if(searchResult.size() < resultsLim + 1){
        cout << "Cannot display more results!" << endl;
        return;
    }
    resultsStart = resultsLim + 1;
    resultsLim = resultsLim + 15;
}

void SearchEngine::prev(){
    if(resultsStart - 15 < 0){
        cout << "Cannot display less results!" << endl;
        return;
    }
    resultsLim = (resultsStart + 14) - 15;
    resultsStart = resultsStart - 15;
}

void SearchEngine::showCounts(){
    cout << endl << "Num of words in tree: " << to_string(index->getWordCount()) << endl;
    cout << "Num of people in tree: " << to_string(index->getPeopleCount()) << endl;
    cout << "Num of orgs in tree: " << to_string(index->getOrgsCount()) << endl << endl;
}

string SearchEngine::toRelativeId(const string& filePath) const {
    if (fileDir.empty()) {
        return filePath;
    }
    error_code ec;
    filesystem::path dataRoot = filesystem::path(fileDir).lexically_normal();
    filesystem::path absFile = filesystem::weakly_canonical(filePath, ec);
    if (ec) {
        absFile = filesystem::absolute(filePath).lexically_normal();
    }
    filesystem::path absRoot = filesystem::weakly_canonical(dataRoot, ec);
    if (ec) {
        absRoot = filesystem::absolute(dataRoot).lexically_normal();
    }

    string fileStr = absFile.generic_string();
    string rootStr = absRoot.generic_string();
    if (!rootStr.empty() && fileStr.rfind(rootStr, 0) == 0) {
        string rel = fileStr.substr(rootStr.size());
        while (!rel.empty() && (rel[0] == '/' || rel[0] == '\\')) {
            rel.erase(rel.begin());
        }
        return rel.empty() ? filePath : rel;
    }
    return filePath;
}

ApiSearchHit SearchEngine::loadHitMetadata(size_t score, const string& filePath) const {
    ApiSearchHit hit;
    hit.score = score;
    hit.file_path = filePath;
    hit.id = toRelativeId(filePath);

    ifstream input(filePath);
    if (!input.is_open()) {
        hit.title = "(could not open article)";
        return hit;
    }
    IStreamWrapper isw(input);
    Document d;
    d.ParseStream(isw);
    if (d.HasParseError()) {
        hit.title = "(invalid article JSON)";
        return hit;
    }
    hit.title = jsonStringOrEmpty(d, "title");
    hit.published = jsonStringOrEmpty(d, "published");
    hit.publication = publicationFromDoc(d);
    return hit;
}

ApiSearchPage SearchEngine::apiSearch(const string& query, int page, int pageSize) {
    if (page < 0) {
        page = 0;
    }
    if (pageSize < 1) {
        pageSize = 15;
    }
    if (pageSize > 50) {
        pageSize = 50;
    }

    ApiSearchPage response;
    response.query = query;
    response.page = page;
    response.page_size = pageSize;

    auto start = chrono::steady_clock::now();
    vector<pair<size_t, string>> results = theQueryEngine->superSearch(query);
    auto end = chrono::steady_clock::now();
    response.took_seconds = chrono::duration<double>(end - start).count();
    response.total = results.size();

    if (results.empty()) {
        return response;
    }

    size_t startIdx = static_cast<size_t>(page) * static_cast<size_t>(pageSize);
    if (startIdx >= results.size()) {
        return response;
    }
    size_t endIdx = min(results.size(), startIdx + static_cast<size_t>(pageSize));
    response.results.reserve(endIdx - startIdx);
    for (size_t i = startIdx; i < endIdx; ++i) {
        response.results.push_back(loadHitMetadata(results[i].first, results[i].second));
    }
    return response;
}

string SearchEngine::apiGetArticleText(const string& idOrPath) const {
    if (idOrPath.empty()) {
        throw invalid_argument("Missing article id");
    }

    error_code ec;
    filesystem::path candidate;

    // Prefer resolving relative ids under the data root.
    if (!fileDir.empty()) {
        filesystem::path underRoot = filesystem::path(fileDir) / idOrPath;
        candidate = filesystem::weakly_canonical(underRoot, ec);
        if (ec || !filesystem::exists(candidate)) {
            candidate = filesystem::weakly_canonical(idOrPath, ec);
        }
    } else {
        candidate = filesystem::weakly_canonical(idOrPath, ec);
    }

    if (ec || !filesystem::exists(candidate) || !filesystem::is_regular_file(candidate)) {
        throw runtime_error("Article not found");
    }

    // Path traversal guard: article must live under the indexed data directory.
    if (!fileDir.empty()) {
        filesystem::path root = filesystem::weakly_canonical(fileDir, ec);
        if (ec) {
            root = filesystem::absolute(fileDir).lexically_normal();
        }
        string rootStr = root.generic_string();
        string fileStr = candidate.generic_string();
        if (fileStr.rfind(rootStr, 0) != 0) {
            throw runtime_error("Article path outside data directory");
        }
    }

    ifstream input(candidate);
    if (!input.is_open()) {
        throw runtime_error("Could not open article");
    }
    IStreamWrapper isw(input);
    Document d;
    d.ParseStream(isw);
    if (d.HasParseError()) {
        throw runtime_error("Invalid article JSON");
    }
    if (!d.HasMember("text") || !d["text"].IsString()) {
        throw runtime_error("Article has no text field");
    }
    return d["text"].GetString();
}

ApiIndexStats SearchEngine::apiGetStats() const {
    ApiIndexStats stats;
    stats.word_count = index->getWordCount();
    stats.people_count = index->getPeopleCount();
    stats.org_count = index->getOrgsCount();
    stats.data_dir = fileDir;
    return stats;
}
