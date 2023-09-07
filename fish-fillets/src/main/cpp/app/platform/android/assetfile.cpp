#include "assetfile.h"

AssetFile::AssetFile(const std::string& path, AAssetManager* assets) :
        m_assets(assets),
        m_path(path)
{ }

bool AssetFile::exists() const {
    return exists(m_path);
}

bool AssetFile::exists(const std::filesystem::path& path) const {
    return (bool) ndk::Asset{m_assets, path.c_str(), AASSET_MODE_UNKNOWN};
}

std::string AssetFile::read() const {
    ndk::Asset asset{m_assets, m_path.c_str(), AASSET_MODE_BUFFER};
    if(!asset)
        Log::fatal("System file not found: ", m_path);
    auto size = AAsset_getLength(asset);
    auto buffer = static_cast<const char*>(AAsset_getBuffer(asset));
    std::string ret(buffer, buffer + size);
    return ret;
}

bool AssetFile::write(const std::string& data) const {
    Log::fatal("Attempt to write to system file.");
}

std::string AssetFile::fullPath() const {
    throw std::logic_error("AssetFile::fullPath unimplemented");
}

ndk::Asset AssetFile::asset(int mode) const {
    return ndk::Asset{m_assets, m_path.c_str(), mode};
}
