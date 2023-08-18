#include "subsystem/files.h"
#include "files.h"
#include "ainstance.h"

std::unique_ptr<IFile> AndroidFiles::system(const std::string& path) {
    return std::make_unique<SystemFile>(path, m_app->activity->assetManager);
}

std::unique_ptr<IFile> AndroidFiles::user(const std::string& path) {
    return std::make_unique<UserFile>(path, m_app->activity->externalDataPath);
}
