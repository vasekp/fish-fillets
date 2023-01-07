#ifndef FF_LINUX_INSTANCE_H
#define FF_LINUX_INSTANCE_H

#include "instance.h"
#include "xlib-fenced.h"
#include "./files.h"
#include "./input.h"

class XInstance : public Instance {
    LinuxFiles m_files;
    XInput m_input;

public:
    Window m_window;

    XInstance(Window window) : m_files(), m_input(*this), m_window(window) { init(); }

    void* window() override { return reinterpret_cast<void*>(m_window); }

    LinuxFiles& files() override { return m_files; }

    XInput& inputSource() override { return m_input; }
};

#endif //FF_LINUX_INSTANCE_H
