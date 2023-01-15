#ifndef FF_ANDROID_FILES_H
#define FF_ANDROID_FILES_H

#include "subsystem/files.h"
#include "./files/system.h"
#include "./files/user.h"

class AndroidFiles : public IFiles {
    android_app* m_app;

public:
    AndroidFiles(android_app* app) : m_app(app) { }

    std::unique_ptr<IFile> system(const std::string& path) override;
    std::unique_ptr<IFile> user(const std::string& path) override;
};

#endif //FF_ANDROID_FILES_H