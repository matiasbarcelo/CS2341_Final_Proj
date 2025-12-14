#ifndef PATHUTILS_H
#define PATHUTILS_H

#include <filesystem>

namespace util {
    // Returns the directory containing the running executable, or an empty path on failure
    std::filesystem::path getExecutableDir();
}

#endif
