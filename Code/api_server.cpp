/**
 * SuperSearch HTTP API host.
 *
 * Loads the same SearchEngine / QueryEngine / IndexHandler pipeline as the CLI,
 * and exposes it over HTTP for a browser demo (e.g. personal website on GitHub Pages).
 *
 * Endpoints:
 *   GET  /api/health
 *   GET  /api/stats
 *   GET  /api/search?q=...&page=0&page_size=15
 *   GET  /api/article?id=coll_1/news_....json
 *   OPTIONS *  (CORS preflight)
 *
 * Environment:
 *   PORT                 listen port (default 8080)
 *   SUPERSEARCH_DATA_DIR directory of JSON articles (default: ../sample_data or sample_data)
 *   SUPERSEARCH_CORS     allowed Origin, or * (default *)
 *   HOST                 bind address (default 0.0.0.0)
 */

// cpp-httplib must be included before any "using namespace std" (Windows byte conflict).
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include "third_party/httplib.h"

#include "SearchEngine.h"

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::invalid_argument;
using std::lock_guard;
using std::mutex;
using std::ostringstream;
using std::string;
using std::vector;

namespace {

string envOr(const char* key, const string& fallback) {
    const char* v = getenv(key);
    if (v == nullptr || v[0] == '\0') {
        return fallback;
    }
    return string(v);
}

string jsonEscape(const string& s) {
    string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

string resolveDataDir() {
    string fromEnv = envOr("SUPERSEARCH_DATA_DIR", "");
    if (!fromEnv.empty() && filesystem::exists(fromEnv) && filesystem::is_directory(fromEnv)) {
        return filesystem::absolute(fromEnv).lexically_normal().string();
    }

    vector<filesystem::path> candidates = {
        filesystem::path("sample_data"),
        filesystem::path("../sample_data"),
        filesystem::path("../../sample_data"),
        filesystem::path("Code/sample_data"),
    };

    for (const auto& c : candidates) {
        if (filesystem::exists(c) && filesystem::is_directory(c)) {
            return filesystem::absolute(c).lexically_normal().string();
        }
    }
    return "";
}

void applyCors(httplib::Response& res, const string& corsOrigin) {
    res.set_header("Access-Control-Allow-Origin", corsOrigin);
    res.set_header("Access-Control-Allow-Methods", "GET, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.set_header("Access-Control-Max-Age", "86400");
}

string statsToJson(const ApiIndexStats& stats) {
    ostringstream oss;
    oss << "{"
        << "\"word_count\":" << stats.word_count << ","
        << "\"people_count\":" << stats.people_count << ","
        << "\"org_count\":" << stats.org_count << ","
        << "\"data_dir\":\"" << jsonEscape(stats.data_dir) << "\""
        << "}";
    return oss.str();
}

string searchPageToJson(const ApiSearchPage& page) {
    ostringstream oss;
    oss << "{"
        << "\"query\":\"" << jsonEscape(page.query) << "\","
        << "\"total\":" << page.total << ","
        << "\"page\":" << page.page << ","
        << "\"page_size\":" << page.page_size << ","
        << "\"took_seconds\":" << page.took_seconds << ","
        << "\"results\":[";
    for (size_t i = 0; i < page.results.size(); ++i) {
        const auto& r = page.results[i];
        if (i > 0) {
            oss << ",";
        }
        oss << "{"
            << "\"index\":" << (static_cast<size_t>(page.page) * static_cast<size_t>(page.page_size) + i) << ","
            << "\"score\":" << r.score << ","
            << "\"id\":\"" << jsonEscape(r.id) << "\","
            << "\"title\":\"" << jsonEscape(r.title) << "\","
            << "\"publication\":\"" << jsonEscape(r.publication) << "\","
            << "\"published\":\"" << jsonEscape(r.published) << "\""
            << "}";
    }
    oss << "]}";
    return oss.str();
}

int parseIntParam(const httplib::Request& req, const char* key, int defaultValue) {
    if (!req.has_param(key)) {
        return defaultValue;
    }
    try {
        return stoi(req.get_param_value(key));
    } catch (...) {
        return defaultValue;
    }
}

} // namespace

int main() {
    const string host = envOr("HOST", "0.0.0.0");
    const int port = stoi(envOr("PORT", "8080"));
    const string corsOrigin = envOr("SUPERSEARCH_CORS", "*");
    const string dataDir = resolveDataDir();

    if (dataDir.empty()) {
        cerr << "ERROR: Could not find sample_data (or SUPERSEARCH_DATA_DIR). "
             << "Set SUPERSEARCH_DATA_DIR to a directory of SuperSearch JSON articles." << endl;
        return 1;
    }

    cout << "SuperSearch API host" << endl;
    cout << "  data dir: " << dataDir << endl;
    cout << "  indexing JSON articles (same DocParser path as the CLI)..." << endl;

    auto loadStart = chrono::steady_clock::now();
    SearchEngine engine(false, dataDir);
    auto loadEnd = chrono::steady_clock::now();
    double loadSeconds = chrono::duration<double>(loadEnd - loadStart).count();

    ApiIndexStats stats = engine.apiGetStats();
    cout << "  index ready in " << loadSeconds << "s" << endl;
    cout << "  words=" << stats.word_count
         << " people=" << stats.people_count
         << " orgs=" << stats.org_count << endl;

    // Protect engine use across concurrent HTTP handlers (index is read-only after load;
    // QueryEngine/SearchEngine helpers are not explicitly thread-safe).
    mutex engineMutex;

    httplib::Server svr;

    auto withCors = [&](httplib::Response& res) {
        applyCors(res, corsOrigin);
    };

    svr.Options(R"(/.*)", [&](const httplib::Request&, httplib::Response& res) {
        withCors(res);
        res.status = 204;
    });

    svr.Get("/api/health", [&](const httplib::Request&, httplib::Response& res) {
        withCors(res);
        ostringstream oss;
        oss << "{"
            << "\"status\":\"ok\","
            << "\"service\":\"supersearch-api\","
            << "\"engine\":\"CS2341 SuperSearch (C++)\","
            << "\"index_load_seconds\":" << loadSeconds
            << "}";
        res.set_content(oss.str(), "application/json; charset=utf-8");
    });

    svr.Get("/api/stats", [&](const httplib::Request&, httplib::Response& res) {
        withCors(res);
        lock_guard<mutex> lock(engineMutex);
        res.set_content(statsToJson(engine.apiGetStats()), "application/json; charset=utf-8");
    });

    svr.Get("/api/search", [&](const httplib::Request& req, httplib::Response& res) {
        withCors(res);
        if (!req.has_param("q")) {
            res.status = 400;
            res.set_content("{\"error\":\"Missing required query parameter q\"}", "application/json; charset=utf-8");
            return;
        }
        string q = req.get_param_value("q");
        int page = parseIntParam(req, "page", 0);
        int pageSize = parseIntParam(req, "page_size", 15);

        try {
            lock_guard<mutex> lock(engineMutex);
            ApiSearchPage result = engine.apiSearch(q, page, pageSize);
            res.set_content(searchPageToJson(result), "application/json; charset=utf-8");
        } catch (const exception& e) {
            res.status = 500;
            res.set_content(string("{\"error\":\"") + jsonEscape(e.what()) + "\"}", "application/json; charset=utf-8");
        }
    });

    svr.Get("/api/article", [&](const httplib::Request& req, httplib::Response& res) {
        withCors(res);
        if (!req.has_param("id")) {
            res.status = 400;
            res.set_content("{\"error\":\"Missing required query parameter id\"}", "application/json; charset=utf-8");
            return;
        }
        string id = req.get_param_value("id");
        try {
            lock_guard<mutex> lock(engineMutex);
            string text = engine.apiGetArticleText(id);
            ostringstream oss;
            oss << "{"
                << "\"id\":\"" << jsonEscape(id) << "\","
                << "\"text\":\"" << jsonEscape(text) << "\""
                << "}";
            res.set_content(oss.str(), "application/json; charset=utf-8");
        } catch (const invalid_argument& e) {
            res.status = 400;
            res.set_content(string("{\"error\":\"") + jsonEscape(e.what()) + "\"}", "application/json; charset=utf-8");
        } catch (const exception& e) {
            res.status = 404;
            res.set_content(string("{\"error\":\"") + jsonEscape(e.what()) + "\"}", "application/json; charset=utf-8");
        }
    });

    svr.Get("/", [&](const httplib::Request&, httplib::Response& res) {
        withCors(res);
        res.set_content(
            R"html(<!DOCTYPE html>
<html><head><meta charset="utf-8"><title>SuperSearch API</title></head>
<body style="font-family: system-ui, sans-serif; max-width: 40rem; margin: 2rem auto;">
  <h1>SuperSearch API</h1>
  <p>This host runs the CS2341 SuperSearch C++ engine over HTTP.</p>
  <ul>
    <li><a href="/api/health">/api/health</a></li>
    <li><a href="/api/stats">/api/stats</a></li>
    <li><a href="/api/search?q=brexit">/api/search?q=brexit</a></li>
  </ul>
  <p>Query syntax matches the CLI: words, <code>PERSON: name</code>, <code>ORG: name</code>.</p>
</body></html>)html",
            "text/html; charset=utf-8");
    });

    cout << "Listening on http://" << host << ":" << port << endl;
    if (!svr.listen(host.c_str(), port)) {
        cerr << "Failed to bind " << host << ":" << port << endl;
        return 1;
    }
    return 0;
}
