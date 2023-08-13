#ifndef FF_IOS_FILES_H
#define FF_IOS_FILES_H

#include "subsystem/files.h"
#include "./files/system.h"
#include "./files/user.h"

class IOSFiles : public IFiles {
    std::filesystem::path m_docs;
    std::filesystem::path m_bundle;

public:
    IOSFiles();

    std::unique_ptr<IFile> system(const std::string& path) override;
    std::unique_ptr<IFile> user(const std::string& path) override;
};

#endif //FF_IOS_FILES_H
