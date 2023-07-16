#include "xinstance.h"
#include <locale>

XInstance::XInstance(Window window) :
    Instance(std::make_unique<LinuxFiles>()),
    m_input(*this),
    m_window(window)
{ }

void* XInstance::window() {
    return reinterpret_cast<void*>(m_window);
}

std::string XInstance::lang() {
    auto locale = std::locale("").name();
    if(locale.length() < 2)
        return "";
    else
        return locale.substr(0, 2);
}
