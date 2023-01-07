#ifndef FF_LINUX_INSTANCE_H
#define FF_LINUX_INSTANCE_H

#include "instance.h"
#include "xlib-fenced.h"
#include "./files.h"

class XInstance : public Instance {
public:
    Window m_window;

    XInstance(Window window) : Instance(std::make_unique<LinuxFiles>()), m_window(window) { }

    void* window() override { return reinterpret_cast<void*>(m_window); }
};

#endif //FF_LINUX_INSTANCE_H
