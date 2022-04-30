#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/decoders.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"

Instance::Instance(android_app* _app) : app(_app), live(false) {
    files = std::make_unique<Files>(this);
    decoders = std::make_unique<Decoders>(app);
}