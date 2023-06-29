#ifndef FISH_FILLETS_FILES_SYSTEM_H
#define FISH_FILLETS_FILES_SYSTEM_H

#include <android_native_app_glue.h>
#include "api/ndk.h"
#include "subsystem/files.h"

class SystemFile : public IFile {
    AAssetManager *m_assets;
    std::filesystem::path m_path;

public:
    SystemFile(const std::string& path, AAssetManager* assets);

    std::string path() const { return m_path; }
    bool exists() const override;
    std::string read() const override;
    bool write(const std::string &data) const override;
    ndk::Asset asset(int mode = AASSET_MODE_UNKNOWN) const;

private:
    bool exists(const std::filesystem::path& path) const;
};

#endif //FISH_FILLETS_FILES_SYSTEM_H
