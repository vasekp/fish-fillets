#include "subsystem/files.h"

SystemFile::SystemFile(std::string path, AAssetManager* assets) :
        m_assets(assets),
        m_path(localize(path))
{ }

bool SystemFile::exists() const {
    return (bool) ndk::Asset{m_assets, m_path.c_str(), AASSET_MODE_UNKNOWN};
}

std::string SystemFile::read() const {
    ndk::Asset asset{m_assets, m_path.c_str(), AASSET_MODE_BUFFER};
    assert(asset);
    auto size = AAsset_getLength(asset);
    auto buffer = static_cast<const char *>(AAsset_getBuffer(asset));
    std::string ret(buffer, buffer + size);
    return ret;
}

ndk::Asset SystemFile::asset(int mode) const {
    return ndk::Asset{m_assets, m_path.c_str(), mode};
}

std::filesystem::path SystemFile::localize(const std::filesystem::path& base) {
    // TODO
    return base;
}