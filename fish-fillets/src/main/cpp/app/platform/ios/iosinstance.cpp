#include "iosinstance.h"
#include "game/screens/screenmanager.h"
#include "glue.h"
#include <CoreFoundation/CFLocale.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFString.h>

IOSInstance::IOSInstance(Glue& glue) :
    Instance(std::make_unique<IOSFiles>()),
    live(false),
    m_glue(glue),
    m_input(*this)
{
    init();
}

std::string IOSInstance::lang() {
    auto array = CFLocaleCopyPreferredLanguages();
    auto first = (CFStringRef)CFArrayGetValueAtIndex(array, 0);
    auto bufSize = CFStringGetLength(first) * 4 + 1;
    std::vector<char> buffer{};
    buffer.resize(bufSize);
    CFStringGetCString(first, buffer.data(), bufSize, kCFStringEncodingASCII);
    CFRelease(first);
    CFRelease(array);
    std::string prefLocale(buffer.data());
    Log::debug<Log::platform>("Preferred locale: ", prefLocale);
    return prefLocale.size() >= 2 ? prefLocale.substr(0, 2) : "";
}
