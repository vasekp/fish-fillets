#ifndef FISH_FILLETS_PLATFORM_INSTANCE_H
#define FISH_FILLETS_PLATFORM_INSTANCE_H

#ifdef __ANDROID__
#include "platform/android/ainstance.h"
#elifdef __linux__
#include "platform/linux/xinstance.h"
#else

#include <EGL/egl.h>

struct PlatformInstance {
    template<typename T>
    PlatformInstance(Instance&, T) { }
    void quit() { }
    NativeWindowType window() { return { }; }
};

struct PlatformData { };

#endif

#endif //FISH_FILLETS_PLATFORM_INSTANCE_H
