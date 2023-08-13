#include "./files.h"
#include <CoreFoundation/CFBundle.h>

IOSFiles::IOSFiles() {
    m_docs = std::filesystem::path(std::getenv("HOME")) / "Documents";
    auto bundleURL = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    auto path = CFURLCopyPath(bundleURL);
    auto pathStr = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);
    CFRelease(path);
    CFRelease(bundleURL);
    m_bundle = std::filesystem::path{pathStr};
}

std::unique_ptr<IFile> LinuxFiles::system(const std::string& path) {
    return std::make_unique<SystemFile>(path, m_bundle);
}

std::unique_ptr<IFile> LinuxFiles::user(const std::string& path) {
    return std::make_unique<UserFile>(path, m_docs);
}
