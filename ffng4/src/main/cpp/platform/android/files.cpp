#include "subsystem/files.h"
#include "./files.h"
#include "ainstance.h"

SystemFile Files::system(const std::string& path) const {
    return {path, m_instance.platform().app->activity->assetManager};
}

UserFile Files::user(const std::string& path) const {
    return {path, m_instance.platform().app->activity->externalDataPath};
}
