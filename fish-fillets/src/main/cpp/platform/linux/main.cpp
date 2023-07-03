#include "common.h"
#include "xinstance.h"
#include "subsystem/graphics.h"
#include "subsystem/persist.h"
#include "game/screens/screenmanager.h"
#include "alsasink.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>

#include <iostream>

int main(int argc, char **argv) {
    Display* dpy = XOpenDisplay(nullptr);
    if(!dpy)
        std::cerr << "cannot connect to X server";
    Window root = DefaultRootWindow(dpy);

    XSetWindowAttributes swa;
    swa.event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask 
        |
        KeyPressMask | KeyReleaseMask | StructureNotifyMask;
    Window win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0,
            CopyFromParent, InputOutput, CopyFromParent, CWEventMask, 
            &swa);

    XSetWindowAttributes xattr;
    xattr.override_redirect = False;
    XChangeWindowAttributes(dpy, win, CWOverrideRedirect, &xattr);

    int one = 1;
    XChangeProperty(dpy, win,
            XInternAtom(dpy, "_HILDON_NON_COMPOSITED_WINDOW", False),
            XA_INTEGER, 32, PropModeReplace, (unsigned char*)&one,  1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(dpy, win, &hints);

    auto wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);

    XkbSetDetectableAutoRepeat(dpy, True, nullptr);

    XInstance instance{win};
    AlsaSink sink{instance.audio()};

    instance.graphics().activate();
    bool intro = !instance.persist().get<int>("intro", 0);
    instance.screens().startMode(intro ? ScreenManager::Mode::Intro : ScreenManager::Mode::WorldMap);
    if(intro)
        instance.persist().set("intro", 1);
    instance.screens().drawFrame();

    XMapWindow(dpy, win);
    XFlush(dpy);

    instance.screens().resume();
    instance.running = true;

    try {
        Log::info("Main loop");
        int lastWidth = 0, lastHeight = 0;
        while(instance.running) {
            while(XPending(dpy)) {
                XEvent event;
                XNextEvent(dpy, &event);
                switch(event.type) {
                    case KeyPress: [[fallthrough]];
                    case KeyRelease:
                        instance.inputSource().keyEvent(event.xkey);
                        break;
                    case ButtonPress: [[fallthrough]];
                    case ButtonRelease:
                        instance.inputSource().buttonEvent(event.xbutton);
                        break;
                    case MotionNotify:
                        instance.inputSource().motionEvent(event.xmotion);
                        break;
                    case ConfigureNotify:
                        {
                            if(event.xconfigure.width == lastWidth && event.xconfigure.height == lastHeight)
                                break;
                            Log::debug("Resize: ", event.xconfigure.width, "×", event.xconfigure.height);
                            instance.graphics().setViewport({0, 0}, {event.xconfigure.width, event.xconfigure.height});
                            lastWidth = event.xconfigure.width;
                            lastHeight = event.xconfigure.height;
                            break;
                        }
                    case ClientMessage:
                        if((Atom)event.xclient.data.l[0] == wmDeleteMessage) {
                            Log::info("Quitting");
                            instance.running = false;
                        }
                        break;
                    default:
                        break;
                }
            }

            instance.inputSource().ping();
            instance.screens().drawFrame();
        }
    } catch(std::exception& e) {
        Log::error("Caught exception ", e.what(), ", exiting");
        return 1;
    }
}
