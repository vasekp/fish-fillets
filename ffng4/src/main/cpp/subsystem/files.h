#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "instance.h"

#include <filesystem>
#include <android_native_app_glue.h>

#include "files/system.h"
#include "files/user.h"

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