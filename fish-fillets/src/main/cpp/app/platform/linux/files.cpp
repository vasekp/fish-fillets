#include "./files.h"

std::unique_ptr<IFile> LinuxFiles::system(const std::string& path) {
    return std::make_unique<SystemFile>(path, "/home/vasek/prog/android/fish-fillets/fish-fillets/src/main/assets/");
}

std::unique_ptr<IFile> LinuxFiles::user(const std::string& path) {
    return std::make_unique<UserFile>(path, "");
}
