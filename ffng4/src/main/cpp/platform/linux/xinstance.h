#ifndef FF_LINUX_INSTANCE_H
#define FF_LINUX_INSTANCE_H

#include "instance.h"
#include "xlib-fenced.h"
#include "./files.h"
#include "./input.h"

class XInstance : public Instance {
public:
    Window m_window;

    XInstance(Window window) : Instance(std::make_unique<LinuxFiles>(), std::make_unique<XInput>(*this)), m_window(window) { }

    void* window() override { return reinterpret_cast<void*>(m_window); }
    
    XInput& xinput() { return dynamic_cast<XInput&>(input()); }
};

#endif //FF_LINUX_INSTANCE_H
