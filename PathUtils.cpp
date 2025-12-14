#include "PathUtils.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

using namespace std;

namespace util {
    std::filesystem::path getExecutableDir(){
        try{
#ifdef _WIN32
            char buf[MAX_PATH];
            DWORD len = GetModuleFileNameA(NULL, buf, MAX_PATH);
            if(len == 0) return std::filesystem::path();
            return std::filesystem::path(std::string(buf)).parent_path();
#else
            char buf[PATH_MAX];
            ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf)-1);
            if(len == -1) return std::filesystem::path();
            buf[len] = '\0';
            return std::filesystem::path(std::string(buf)).parent_path();
#endif
        }
        catch(...){
            return std::filesystem::path();
        }
    }
}
