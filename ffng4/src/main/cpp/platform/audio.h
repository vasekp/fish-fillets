#ifndef FISH_FILLETS_PLATFORM_AUDIO_H
#define FISH_FILLETS_PLATFORM_AUDIO_H

#ifdef __ANDROID__
#include "platform/android/audio.h"
#else

class AudioSink {
public:
    AudioSink(Audio& iface) { }

    void start() { }
    void stop() { }
};

#endif

#endif //FISH_FILLETS_PLATFORM_AUDIO_H
