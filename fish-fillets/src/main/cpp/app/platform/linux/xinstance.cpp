#include "xinstance.h"
#include "subsystem/graphics.h"
#include "game/screens/screenmanager.h"
#include "subsystem/graphics/graphicsbackend.h"

#include <locale>

XInstance::XInstance(Display* dpy, Window win) :
    Instance(std::make_unique<LinuxFiles>()),
    m_display(dpy),
    m_window(win),
    m_input(*this),
    m_sink(audio()),
    m_quit(false)
{
    m_atoms["WM_DELETE_WINDOW"] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    m_atoms["WM_STATE"] = XInternAtom(dpy, "WM_STATE", False);
    XSetWMProtocols(dpy, win, &m_atoms["WM_DELETE_WINDOW"], 1);
    init();

#ifdef FISH_FILLETS_USE_VULKAN
    auto display = vulkan::Display(dpy, win);
#else
    auto display = ogl::Display(win);
#endif
    graphics().activate(std::move(display));
}

std::string XInstance::lang() {
    auto locale = std::locale("").name();
    if(locale.length() < 2)
        return "";
    else
        return locale.substr(0, 2);
}

void XInstance::dispatchEvent(const XEvent& event) {
    switch(event.type) {
        case KeyPress: [[fallthrough]];
        case KeyRelease:
            inputSource().keyEvent(event.xkey);
            break;
        case ButtonPress: [[fallthrough]];
        case ButtonRelease:
            inputSource().buttonEvent(event.xbutton);
            break;
        case MotionNotify:
            inputSource().motionEvent(event.xmotion);
            break;
        case ConfigureNotify:
            {
                USize size{(unsigned)event.xconfigure.width, (unsigned)event.xconfigure.height};
                if(size == m_lastSize)
                    break;
                Log::debug<Log::platform>("Resize: ", size);
                graphics().setViewport({0, 0}, ICoords{size});
                m_lastSize = size;
                break;
            }
        case ClientMessage:
            if((Atom)event.xclient.data.l[0] == m_atoms["WM_DELETE_WINDOW"]) {
                Log::info<Log::lifecycle>("Window closed");
                quit();
            }
            break;
        case PropertyNotify:
            {
                if(event.xproperty.atom != m_atoms["WM_STATE"])
                    break;
                Atom type;
                int format;
                unsigned long* state;
                unsigned long after;
                unsigned long length;
                do {
                    auto status = XGetWindowProperty(m_display, m_window, event.xproperty.atom,
                                                0, 1, 0,
                                                m_atoms["WM_STATE"], &type, &format,
                                                &length, &after, (unsigned char**)&state);
                } while(!length);
                Log::debug<Log::platform>("X window state: ", *state);
                if(*state == IconicState)
                    pause();
                else
                    run();
                XFree(state);
            }
            break;
        default:
            break;
    }
}

void XInstance::own_run() {
    m_sink.resume();
}

void XInstance::own_pause() {
    m_sink.pause();
}

void XInstance::own_quit() {
    m_quit = true;
}
