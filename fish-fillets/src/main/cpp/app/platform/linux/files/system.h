#ifndef FISH_FILLETS_FILES_SYSTEM_H
#define FISH_FILLETS_FILES_SYSTEM_H

#include "subsystem/files.h"

class SystemFile : public IFile {
    std::filesystem::path m_relPath;
    std::filesystem::path m_fullPath;

public:
    SystemFile(std::string path, const std::filesystem::path& basePath);

    std::string path() const { return m_relPath; }
    std::string fullPath() const { return m_fullPath; }
    bool exists() const override;
    std::string read() const override;
    bool write(const std::string& data) const override;
};

#endif //FISH_FILLETS_FILES_SYSTEM_H
