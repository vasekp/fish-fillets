#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "common.h"
#include <android/asset_manager_jni.h>
#include <filesystem>
#include <sstream>
#include <fstream>

class AbstractFile {
public:
    virtual bool exists() const = 0;
    virtual std::string read() const = 0;
    virtual bool write(const std::string& data) const = 0;
};

class Asset {
    AAsset *asset;

public:
    Asset(AAssetManager *assets, const char* filename, int mode) :
        asset(AAssetManager_open(assets, filename, mode))
    { }

    ~Asset() {
        AAsset_close(asset);
    }

    bool valid() const {
        return asset != nullptr;
    }

    AAsset* operator *() {
        return asset;
    }
};

class SystemFile : public AbstractFile {
    AAssetManager *assets;
    std::filesystem::path path;

public:
    SystemFile(std::string _path, const Instance& instance) :
        assets(instance.app->activity->assetManager),
        path(localize(_path))
    { }

    virtual bool exists() const {
        return Asset{assets, path.c_str(), AASSET_MODE_UNKNOWN}.valid();
    }

    virtual std::string read() const {
        Asset asset{assets, path.c_str(), AASSET_MODE_BUFFER};
        assert(asset.valid());
        auto size = AAsset_getLength(*asset);
        auto buffer = static_cast<const char *>(AAsset_getBuffer(*asset));
        LOGD("read %ld bytes from %s", size, path.c_str());
        std::string ret(buffer, buffer + size);
        return ret;
    }

    virtual bool write(const std::string &data) const {
        throw std::logic_error("write attempt to internal file");
    }

    Asset asset(int mode = AASSET_MODE_UNKNOWN) const {
        return Asset{assets, path.c_str(), mode};
    }

private:
    std::filesystem::path localize(const std::filesystem::path& base) {
        // TODO
        return base;
    }
};

class UserFile : public AbstractFile {
    std::filesystem::path basePath;
    std::filesystem::path fullPath;

public:
    UserFile(std::string _path, const Instance& instance) :
        basePath(_path),
        fullPath(std::filesystem::path{instance.app->activity->externalDataPath} / _path)
    { }

    virtual bool exists() const {
        return std::filesystem::exists(fullPath);
    }

    virtual std::string read() const {
        std::ostringstream oss;
        std::ifstream ifs{fullPath};
        oss << ifs.rdbuf();
        return oss.str();
    }

    virtual bool write(const std::string &data) const {
        std::filesystem::path path{fullPath};
        std::filesystem::create_directories(path.parent_path());
        std::ofstream ofs{fullPath};
        ofs << data;
        return true;
    }
};

#endif //FISH_FILLETS_FILES_H
