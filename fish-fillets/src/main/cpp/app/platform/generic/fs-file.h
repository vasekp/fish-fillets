#ifndef FISH_FILLETS_FS_FILE_H
#define FISH_FILLETS_FS_FILE_H

#include "subsystem/files.h"

class FilesystemFile : public IFile {
    std::filesystem::path m_relPath;
    std::filesystem::path m_fullPath;

public:
    FilesystemFile(std::string path, const std::filesystem::path& basePath);

    std::string relPath() const override { return m_relPath; }
    std::string fullPath() const override { return m_fullPath; }
    bool exists() const override;
    std::string read() const override;
    bool write(const std::string &data) const override;
};

#endif //FISH_FILLETS_FS_FILE_H
