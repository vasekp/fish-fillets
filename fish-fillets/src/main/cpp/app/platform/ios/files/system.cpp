#include "system.h"

#include <sstream>
#include <fstream>

SystemFile::SystemFile(std::string path, const std::filesystem::path& basePath) :
        m_relPath(std::move(path)),
        m_fullPath(basePath / m_relPath)
{ }

bool SystemFile::exists() const {
    return std::filesystem::exists(m_fullPath);
}

std::string SystemFile::read() const {
    if(!exists())
        Log::fatal("System file not found: ", m_fullPath);
    std::ostringstream oss;
    std::ifstream ifs{m_fullPath};
    oss << ifs.rdbuf();
    return oss.str();
}

bool SystemFile::write(const std::string &data) const {
    Log::fatal("Attempt to write to system file.");
}
