#ifndef FISH_FILLETS_FILES_USER_H
#define FISH_FILLETS_FILES_USER_H

#include <sstream>
#include <fstream>

class UserFile {
    std::filesystem::path m_relPath;
    std::filesystem::path m_fullPath;

public:
    UserFile(std::string path, const std::filesystem::path& basePath);

    std::string getPath() const { return m_relPath; }

    bool exists() const;
    std::string read() const;
    bool write(const std::string &data) const;
};

#endif //FISH_FILLETS_FILES_USER_H