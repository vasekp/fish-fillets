#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "instance.h"

#include <android_native_app_glue.h>

#include "files/ifile.h"
#include "files/system.h"
#include "files/user.h"

class Files {
    Instance& m_instance;

public:
    Files(Instance& instance) : m_instance(instance) { }

    SystemFile system(const std::string& path) const {
        return {path, m_instance.app()->activity->assetManager};
    }

    UserFile user(const std::string& path) const {
        return {path, m_instance.app()->activity->externalDataPath};
    }
};

#endif //FISH_FILLETS_FILES_H