#include "subsystem/files.h"
#include "./files.h"

SystemFile Files::system(const std::string& path) const {
    return {path, m_instance.app()->activity->assetManager};
}

UserFile Files::user(const std::string& path) const {
    return {path, m_instance.app()->activity->externalDataPath};
}
