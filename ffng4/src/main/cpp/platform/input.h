#ifndef FISH_FILLETS_PLATFORM_INPUT_H
#define FISH_FILLETS_PLATFORM_INPUT_H

#ifdef __ANDROID__
#include "platform/android/input.h"
#elifdef __linux__
#include "platform/linux/input.h"
#else

class PlatformInput : public IInputProvider {
public:
    PlatformInput(Instance&) { }
};

#endif

#endif //FISH_FILLETS_PLATFORM_INPUT_H