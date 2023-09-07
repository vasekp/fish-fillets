#ifndef FISH_FILLETS_ASSETFILE_H
#define FISH_FILLETS_ASSETFILE_H

#include "api/ndk.h"
#include "subsystem/files.h"

class AssetFile : public IFile {
    AAssetManager* m_assets;
    std::filesystem::path m_path;

public:
    AssetFile(const std::string& path, AAssetManager* assets);

    bool exists() const override;
    std::string read() const override;
    bool write(const std::string& data) const override;
    std::string relPath() const override { return m_path; }
    std::string fullPath() const override;
    ndk::Asset asset(int mode = AASSET_MODE_UNKNOWN) const;

private:
    bool exists(const std::filesystem::path& path) const;
};

#endif //FISH_FILLETS_ASSETFILE_H
