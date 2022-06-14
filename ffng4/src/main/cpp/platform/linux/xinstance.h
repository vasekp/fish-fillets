#ifndef FISH_FILLETS_LINUX_INSTANCE_H
#define FISH_FILLETS_LINUX_INSTANCE_H

#include "instance.h"
#include <X11/Xlib.h>

struct PlatformInstance {
    Window m_window;

    PlatformInstance(Instance&, Window window) : m_window(window) { }

    void quit() { }
    auto window() { return m_window; }
};

using PlatformData = Window;

#endif //FISH_FILLETS_LINUX_INSTANCE_H
