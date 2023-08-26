#include "subsystem/files.h"
#include "files.h"
#include "ainstance.h"

AndroidFiles::AndroidFiles(android_app* app) :
        m_assets(app->activity->assetManager),
        m_userPath(app->activity->externalDataPath)
{ }

AndroidFiles::AndroidFiles(JNIEnv* env, jobject jAssets, std::string&& userPath) :
        m_assets(AAssetManager_fromJava(env, jAssets)),
        m_userPath(std::move(userPath))
{ }

std::unique_ptr<IFile> AndroidFiles::system(const std::string& path) {
    return std::make_unique<SystemFile>(path, m_assets);
}

std::unique_ptr<IFile> AndroidFiles::user(const std::string& path) {
    return std::make_unique<UserFile>(path, m_userPath);
}
