#include "subsystem/files.h"

#include <sstream>
#include <fstream>

UserFile::UserFile(std::string _path, const std::filesystem::path& basePath) :
        relPath(std::move(_path)),
        fullPath(basePath / _path)
{ }

bool UserFile::exists() const {
    return std::filesystem::exists(fullPath);
}

std::string UserFile::read() const {
    std::ostringstream oss;
    std::ifstream ifs{fullPath};
    oss << ifs.rdbuf();
    return oss.str();
}

bool UserFile::write(const std::string &data) const {
    std::filesystem::path path{fullPath};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream ofs{fullPath};
    ofs << data;
    return true;
}