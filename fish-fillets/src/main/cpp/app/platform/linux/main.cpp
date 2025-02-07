#include "common.h"
#include "xinstance.h"

std::pair<Display*, Window> initWindow() {
    Display* dpy = XOpenDisplay(nullptr);
    if(!dpy)
        Log::fatal("Cannot connect to X server.");
    Window root = DefaultRootWindow(dpy);

    XSetWindowAttributes swa;
    swa.event_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask 
        | KeyPressMask | KeyReleaseMask
        | StructureNotifyMask | PropertyChangeMask;
    Window win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0,
            CopyFromParent, InputOutput, CopyFromParent, CWEventMask,
            &swa);

    XSetWindowAttributes xattr;
    xattr.override_redirect = False;
    XChangeWindowAttributes(dpy, win, CWOverrideRedirect, &xattr);

    XStoreName(dpy, win, "Fish Fillets");

    int one = 1;
    XChangeProperty(dpy, win,
            XInternAtom(dpy, "_HILDON_NON_COMPOSITED_WINDOW", False),
            XA_INTEGER, 32, PropModeReplace, (unsigned char*)&one,  1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(dpy, win, &hints);

    XkbSetDetectableAutoRepeat(dpy, True, nullptr);

    return {dpy, win};
}

int main(int argc, char** argv) {
    try {
        auto [dpy, win] = initWindow();

        XInstance instance{dpy, win};

        XMapWindow(dpy, win);
        XFlush(dpy);
        instance.run();

        Log::info<Log::lifecycle>("Main loop started");
        while(!instance.closed()) {
            while(XPending(dpy)) {
                XEvent event;
                XNextEvent(dpy, &event);
                instance.dispatchEvent(event);
            }
            instance.updateAndDraw();
        }
        Log::info<Log::lifecycle>("Main loop finished");
    } catch(std::exception& e) {
        Log::error("Caught exception ", e.what(), ", exiting");
        return 1;
    }
}
