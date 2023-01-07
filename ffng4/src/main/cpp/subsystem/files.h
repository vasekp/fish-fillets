#ifndef FISH_FILLETS_FILES_H
#define FISH_FILLETS_FILES_H

#include "instance.h"

class IFile {
public:
    virtual ~IFile() { }

    virtual bool exists() const = 0;
    virtual std::string read() const = 0;
    virtual bool write(const std::string &data) const = 0;
};

class IFiles {
public:
    virtual ~IFiles() { }

    virtual std::unique_ptr<IFile> system(const std::string& path) = 0;
    virtual std::unique_ptr<IFile> user(const std::string& path) = 0;
};

#endif //FISH_FILLETS_FILES_H
