#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "IndexHandler.h"
#include "DocParser.h"
#include "QueryEngine.h"
#include <vector>
#include <utility>
#include <string>
#include <cstddef>

using namespace std;

/** One ranked hit returned by the HTTP API (and available for non-CLI callers). */
struct ApiSearchHit {
    size_t score = 0;
    string id;           // path relative to the data root when possible
    string file_path;    // absolute/resolved path used to open the article JSON
    string title;
    string publication;
    string published;
};

/** Paged search response for the HTTP API. */
struct ApiSearchPage {
    string query;
    size_t total = 0;
    int page = 0;
    int page_size = 15;
    double took_seconds = 0.0;
    vector<ApiSearchHit> results;
};

/** Index stats for /api/health and /api/stats. */
struct ApiIndexStats {
    size_t word_count = 0;
    size_t people_count = 0;
    size_t org_count = 0;
    string data_dir;
};

class SearchEngine{
    private:
        IndexHandler* index;
        DocParser* theDocParser;
        QueryEngine* theQueryEngine;
        vector<pair<size_t, string>> searchResult;
        string fileDir;
        int resultsStart;
        int resultsLim;
        bool persistence;

        ApiSearchHit loadHitMetadata(size_t score, const string& filePath) const;
        string toRelativeId(const string& filePath) const;
    
    public:
        SearchEngine(bool hasPersistance = true, string fileDirectory = "");
        ~SearchEngine();
        void superSearch(string theQuery);
        void displayResults();
        void displayText(string theNum);
        void nextPage();
        void previousPage();
        bool hasSearchResults();
        void next();
        void prev();
        void showCounts();

        /** Same search pipeline as the CLI, but returns structured paged results. */
        ApiSearchPage apiSearch(const string& query, int page = 0, int pageSize = 15);

        /**
         * Load article body for a document id or path returned by apiSearch.
         * Only files under the configured data directory are allowed.
         */
        string apiGetArticleText(const string& idOrPath) const;

        ApiIndexStats apiGetStats() const;
        const string& getDataDir() const { return fileDir; }
};

#endif
