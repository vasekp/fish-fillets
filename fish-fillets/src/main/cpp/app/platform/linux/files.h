#ifndef FF_LINUX_FILES_H
#define FF_LINUX_FILES_H

#include "common.h"
#include "subsystem/files.h"
#include "files/system.h"
#include "files/user.h"

class LinuxFiles : public IFiles {
public:
    std::unique_ptr<IFile> system(const std::string& path) override;
    std::unique_ptr<IFile> user(const std::string& path) override;
};

#endif //FF_LINUX_FILES_H
