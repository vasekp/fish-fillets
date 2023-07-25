#include "xinstance.h"
#include "subsystem/graphics.h"

#include <locale>

XInstance::XInstance(Display* dpy, Window win) :
    Instance(std::make_unique<LinuxFiles>()),
    m_input(*this),
    m_sink(audio())
{
    auto wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);
    m_deleteAtom = wmDeleteMessage;
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
                ICoords size{event.xconfigure.width, event.xconfigure.height};
                if(size == m_lastSize)
                    break;
                Log::debug<Log::platform>("Resize: ", size);
                graphics().setViewport({0, 0}, size);
                m_lastSize = size;
                break;
            }
        case ClientMessage:
            if((Atom)event.xclient.data.l[0] == m_deleteAtom) {
                Log::info<Log::lifecycle>("Quitting");
                running = false;
            }
            break;
        default:
            break;
    }
}
