#ifndef FF_LINUX_INSTANCE_H
#define FF_LINUX_INSTANCE_H

#include "instance.h"
#include "./files.h"
#include "./input.h"
#include <X11/Xlib.h>

class XInstance : public Instance {
    LinuxFiles m_files;
    XInput m_input;

public:
    Window m_window;

    XInstance(Window window) :
        Instance(std::make_unique<LinuxFiles>()),
        m_input(*this),
        m_window(window)
    { }

    void* window() override { return reinterpret_cast<void*>(m_window); }

    XInput& inputSource() override { return m_input; }
};

#endif //FF_LINUX_INSTANCE_H
