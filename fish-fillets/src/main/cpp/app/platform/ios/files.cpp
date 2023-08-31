#include "./files.h"
#include <CoreFoundation/CFBundle.h>

IOSFiles::IOSFiles() {
    m_docs = std::filesystem::path(std::getenv("HOME")) / "Documents";
    auto bundleURL = CFBundleCopyBundleURL(CFBundleGetMainBundle());
    auto path = CFURLCopyPath(bundleURL);
    auto pathStr = CFStringGetCStringPtr(path, kCFStringEncodingUTF8);
    m_bundle = std::filesystem::path{pathStr};
    CFRelease(path);
    CFRelease(bundleURL);
}

std::unique_ptr<IFile> IOSFiles::system(const std::string& path) {
    return std::make_unique<SystemFile>(path, m_bundle);
}

std::unique_ptr<IFile> IOSFiles::user(const std::string& path) {
    return std::make_unique<UserFile>(path, m_docs);
}
