#include "subsystem/files.h"
#include "files.h"
#include "ainstance.h"
#include "platform/generic/fs-file.h"
#include "assetfile.h"

std::unique_ptr<IFile> AndroidFiles::system(const std::string& path) {
    return std::make_unique<AssetFile>(path, m_app->activity->assetManager);
}

std::unique_ptr<IFile> AndroidFiles::user(const std::string& path) {
    return std::make_unique<FilesystemFile>(path, m_app->activity->externalDataPath);
}
