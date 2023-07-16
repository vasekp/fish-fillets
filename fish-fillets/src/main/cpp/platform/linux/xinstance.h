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

    XInstance(Window window);

    void* window() override;
    XInput& inputSource() override { return m_input; }
    std::string lang() override;
};

#endif //FF_LINUX_INSTANCE_H
