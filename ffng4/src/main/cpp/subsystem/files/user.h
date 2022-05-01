#ifndef FISH_FILLETS_FILES_USER_H
#define FISH_FILLETS_FILES_USER_H

#include <sstream>
#include <fstream>

class UserFile {
    std::filesystem::path relPath;
    std::filesystem::path fullPath;

public:
    UserFile(std::string _path, const std::filesystem::path& basePath);

    std::string getPath() const { return relPath; }

    bool exists() const;
    std::string read() const;
    bool write(const std::string &data) const;
};

#endif //FISH_FILLETS_FILES_USER_H