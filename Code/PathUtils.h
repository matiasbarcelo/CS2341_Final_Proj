#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <filesystem>

namespace util {
    // Returns the directory containing the running executable, or an empty path on failure
    std::filesystem::path getExecutableDir();
    // Search for the first file with given filename under start (inclusive) up to maxDepth levels.
    std::filesystem::path findFileInDescendants(const std::filesystem::path &start, const std::string &filename, int maxDepth = 4);
    // Search for the first directory with given name under start (inclusive) up to maxDepth levels.
    std::filesystem::path findDirectoryInDescendants(const std::filesystem::path &start, const std::string &dirname, int maxDepth = 3);
}

#endif
