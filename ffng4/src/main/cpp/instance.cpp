#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"

Instance::Instance(android_app* _app) : app(_app), jni(app), live(false) {
    files = std::make_unique<Files>(this);
    graphics = std::make_unique<Graphics>(this);
    audio = std::make_unique<Audio>(this);
}