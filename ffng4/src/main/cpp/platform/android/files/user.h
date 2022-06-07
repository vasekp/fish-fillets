#ifndef FISH_FILLETS_FILES_USER_H
#define FISH_FILLETS_FILES_USER_H

class UserFile : public IFile {
    std::filesystem::path m_relPath;
    std::filesystem::path m_fullPath;

public:
    UserFile(std::string path, const std::filesystem::path& basePath);

    std::string getPath() const { return m_relPath; }

    bool exists() const override;
    std::string read() const override;
    bool write(const std::string &data) const override;
};

#endif //FISH_FILLETS_FILES_USER_H
