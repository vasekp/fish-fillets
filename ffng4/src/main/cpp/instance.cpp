#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "game/screen.h"

Instance::Instance(android_app* androidApp) : app(androidApp), jni(app), live(false) {
    files = std::make_unique<Files>(this);
    graphics = std::make_unique<Graphics>(this);
    audio = std::make_unique<Audio>(this);
}

GameScreen* Instance::screen() {
    return screens.back().get();
}