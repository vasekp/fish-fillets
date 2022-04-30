#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "instance.h"
#include "platform/ndk.h"

#include <android_native_app_glue.h>
#include <filesystem>
#include <sstream>
#include <fstream>

class SystemFile;
class UserFile;


class SystemFile {
    AAssetManager *assets;
    std::filesystem::path path;

public:
    SystemFile(std::string _path, AAssetManager* _assets) :
        assets(_assets),
        path(localize(_path))
    { }

    std::string getPath() const {
        return path;
    }

    virtual bool exists() const {
        return (bool) ndk::Asset{assets, path.c_str(), AASSET_MODE_UNKNOWN};
    }

    virtual std::string read() const {
        ndk::Asset asset{assets, path.c_str(), AASSET_MODE_BUFFER};
        assert(asset);
        auto size = AAsset_getLength(asset);
        auto buffer = static_cast<const char *>(AAsset_getBuffer(asset));
        std::string ret(buffer, buffer + size);
        return ret;
    }

    ndk::Asset asset(int mode = AASSET_MODE_UNKNOWN) const {
        return ndk::Asset{assets, path.c_str(), mode};
    }

private:
    std::filesystem::path localize(const std::filesystem::path& base) {
        // TODO
        return base;
    }
};

class UserFile {
    std::filesystem::path relPath;
    std::filesystem::path fullPath;

public:
    UserFile(std::string _path, std::filesystem::path basePath) :
        relPath(_path),
        fullPath(basePath / _path)
    { }

    std::string getPath() const {
        return relPath;
    }

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


class Files {
    Instance* instance;

public:
    Files(Instance* instance_) : instance(instance_) { }

    SystemFile system(const std::string& path) const {
        return {path, instance->app->activity->assetManager};
    }

    UserFile user(const std::string& path) const {
        return {path, instance->app->activity->externalDataPath};
    }
};

#endif //FISH_FILLETS_FILES_H