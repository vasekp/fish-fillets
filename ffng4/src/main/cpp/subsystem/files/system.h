#ifndef FISH_FILLETS_FILES_SYSTEM_H
#define FISH_FILLETS_FILES_SYSTEM_H

#include "platform/ndk.h"

class SystemFile {
    AAssetManager *m_assets;
    std::filesystem::path m_path;

public:
    SystemFile(std::string path, AAssetManager* assets);

    std::string getPath() const { return m_path; }

    bool exists() const;
    std::string read() const;
    ndk::Asset asset(int mode = AASSET_MODE_UNKNOWN) const;

private:
    std::filesystem::path localize(const std::filesystem::path& base);
};

#endif //FISH_FILLETS_FILES_SYSTEM_H