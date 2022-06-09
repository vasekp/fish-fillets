#include "common.h"
#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "game/screens/screenmanager.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

int main(int argc, char **argv) {
  try {
    Display* dpy = XOpenDisplay(nullptr);
    if(!dpy)
      Log::fatal("cannot connect to X server");
    Window root = DefaultRootWindow(dpy);
    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;
    Window win = XCreateWindow(dpy, root, 0, 0, 300, 300, 0,
        CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

    int one = 1;
    XChangeProperty(dpy, win,
       XInternAtom(dpy, "_HILDON_NON_COMPOSITED_WINDOW", False),
       XA_INTEGER, 32, PropModeReplace, (unsigned char*)&one,  1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(dpy, win, &hints);

    Instance instance{win};
    instance.screens().startMode(ScreenManager::Mode::WorldMap);

    instance.live = true;
    instance.graphics().activate();
    instance.audio().activate();
    instance.screens().refresh();
    instance.screens().drawFrame();
  } catch(std::exception& e) {
    Log::error("Caught exception ", e.what(), ", exiting");
    return 1;
  }
}
