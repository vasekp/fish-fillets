#include "subsystem/files.h"
#include "system.h"


SystemFile::SystemFile(const std::string& path, AAssetManager* assets) :
        m_assets(assets),
        m_path(localize(path))
{ }

bool SystemFile::exists() const {
    return exists(m_path);
}

bool SystemFile::exists(const std::filesystem::path &path) const {
    return (bool) ndk::Asset{m_assets, path.c_str(), AASSET_MODE_UNKNOWN};
}

std::string SystemFile::read() const {
    ndk::Asset asset{m_assets, m_path.c_str(), AASSET_MODE_BUFFER};
    if(!asset)
        Log::fatal("System file not found: ", m_path);
    auto size = AAsset_getLength(asset);
    auto buffer = static_cast<const char *>(AAsset_getBuffer(asset));
    std::string ret(buffer, buffer + size);
    return ret;
}

bool SystemFile::write(const std::string& data) const {
    Log::fatal("Attempt to write to system file.");
}

ndk::Asset SystemFile::asset(int mode) const {
    return ndk::Asset{m_assets, m_path.c_str(), mode};
}

std::filesystem::path SystemFile::localize(const std::filesystem::path& base) {
    // TODO
    auto override = "override" / base;
    if (exists(override)) {
        Log::debug("override ", base, " -> ", override);
        return override;
    } else
        return base;
}