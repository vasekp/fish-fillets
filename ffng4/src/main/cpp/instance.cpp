#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "graphics.h"
#include "decoders.h"
#include "audio.h"

Instance::Instance(android_app* _app) : app(_app), live(false) { }