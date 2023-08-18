#ifndef FF_LINUX_INSTANCE_H
#define FF_LINUX_INSTANCE_H

#include "instance.h"
#include "./files.h"
#include "./input.h"
#include "alsasink.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

class XInstance : public Instance {
public:
    XInstance(Display* dpy, Window window);

    XInput& inputSource() override { return m_input; }
    std::string lang() override;

    bool closed() { return m_quit; }

    void dispatchEvent(const XEvent& event);

private:
    Display* m_display;
    Window m_window;
    LinuxFiles m_files;
    XInput m_input;
    AlsaSink m_sink;
    USize m_lastSize;
    std::map<std::string, Atom> m_atoms;
    bool m_quit;

    void own_run() override;
    void own_pause() override;
    void own_quit() override;
};

#endif //FF_LINUX_INSTANCE_H
