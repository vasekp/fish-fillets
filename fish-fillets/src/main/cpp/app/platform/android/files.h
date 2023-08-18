#ifndef FF_ANDROID_FILES_H
#define FF_ANDROID_FILES_H

#include "common.h"
#include "subsystem/files.h"
#include "./files/system.h"
#include "./files/user.h"

class AndroidFiles : public IFiles {
public:
    AndroidFiles(android_app* app) : m_app(app) { }

    std::unique_ptr<IFile> system(const std::string& path) override;
    std::unique_ptr<IFile> user(const std::string& path) override;

private:
    android_app* m_app;
};

#endif //FF_ANDROID_FILES_H
