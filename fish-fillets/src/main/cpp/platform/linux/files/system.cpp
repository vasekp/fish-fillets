#include "system.h"

#include <sstream>
#include <fstream>

SystemFile::SystemFile(std::string path, const std::filesystem::path& basePath) :
        m_relPath(std::move(path))
{
    auto overrideRel = "override" / m_relPath;
    auto overrideFull = basePath / overrideRel;
    if(std::filesystem::exists(overrideFull)) {
        Log::debug<Log::platform>("override ", m_relPath, " -> ", overrideRel);
        m_fullPath = overrideFull;
    } else
        m_fullPath = basePath / m_relPath;
}

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
    std::filesystem::path path{m_fullPath};
    std::filesystem::create_directories(path.parent_path());
    std::ofstream ofs{m_fullPath};
    ofs << data;
    return true;
}
