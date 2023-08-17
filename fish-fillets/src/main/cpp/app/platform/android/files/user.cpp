#include "user.h"

#include <sstream>
#include <fstream>

UserFile::UserFile(std::string path, const std::filesystem::path& basePath) :
        m_relPath(std::move(path)),
        m_fullPath(basePath / m_relPath)
{ }

bool UserFile::exists() const {
    return std::filesystem::exists(m_fullPath);
}

std::string UserFile::read() const {
    std::ostringstream oss;
    std::ifstream ifs{m_fullPath};
    oss << ifs.rdbuf();
    return oss.str();
}

bool UserFile::write(const std::string& data) const {
    std::filesystem::path path{m_fullPath};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream ofs{m_fullPath};
    ofs << data;
    return true;
}
