#ifndef FF_ANDROID_FILES_H
#define FF_ANDROID_FILES_H

#include "common.h"
#include "subsystem/files.h"
#include "files/system.h"
#include "files/user.h"
#include "api/jni.h"
#include <android/asset_manager_jni.h>

class AndroidFiles : public IFiles {
public:
    AndroidFiles(android_app* app);
    AndroidFiles(JNIEnv* env, jobject jAssets, std::string&& userPath);

    std::unique_ptr<IFile> system(const std::string& path) override;
    std::unique_ptr<IFile> user(const std::string& path) override;

private:
    AAssetManager* m_assets;
    std::string m_userPath;
};

#endif //FF_ANDROID_FILES_H
