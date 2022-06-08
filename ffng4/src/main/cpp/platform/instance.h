#ifndef FISH_FILLETS_PLATFORM_INSTANCE_H
#define FISH_FILLETS_PLATFORM_INSTANCE_H

#ifdef __ANDROID__
#include "platform/android/ainstance.h"
#else

struct PlatformInstance {
    template<typename T>
    PlatformInstance(Instance&, T) { }
    void quit() { }
};

struct PlatformData { };

#endif

#endif //FISH_FILLETS_PLATFORM_INSTANCE_H
