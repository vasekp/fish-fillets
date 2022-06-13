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
    swa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask | StructureNotifyMask;
    Window win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0,
        CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

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

    Instance instance{win};
    instance.screens().startMode(ScreenManager::Mode::WorldMap);

    instance.live = true;
    instance.graphics().activate();
    instance.audio().activate();
    instance.screens().refresh();
    instance.screens().drawFrame();

    XMapWindow(dpy, win);
    XFlush(dpy);

    instance.screens().resume();
    instance.audio().resume();
    instance.running = true;

    Log::info("Main loop");
    while(instance.running) {
      while(XPending(dpy)) {
        XEvent event;
        XNextEvent(dpy, &event);
        switch(event.type) {
          /*case Expose:
            break;*/
          case ConfigureNotify:
          {
            Log::debug("Resize: ", instance.graphics().display().width(), "×", instance.graphics().display().height());
            instance.graphics().system().notifyDisplayResize();
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

      instance.screens().drawFrame();
    }

  } catch(std::exception& e) {
    Log::error("Caught exception ", e.what(), ", exiting");
    return 1;
  }
}
