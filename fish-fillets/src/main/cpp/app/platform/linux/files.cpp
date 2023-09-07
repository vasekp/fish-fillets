#include "files.h"
#include "platform/generic/fs-file.h"

std::unique_ptr<IFile> LinuxFiles::system(const std::string& path) {
    return std::make_unique<FilesystemFile>(path, "/home/vasek/prog/android/fish-fillets/fish-fillets/src/main/assets/"); // TODO
}

std::unique_ptr<IFile> LinuxFiles::user(const std::string& path) {
    return std::make_unique<FilesystemFile>(path, ""); // TODO
}
