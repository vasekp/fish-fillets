#ifndef FISH_FILLETS_DECODERS_H
#define FISH_FILLETS_DECODERS_H

#include "common.h"
#include "platform/jni.h"

#include <android_native_app_glue.h>
#include <jni.h>

namespace ogl {
    class Texture;
}

class Decoders {
    jni::Env jni;

public:
    Decoders(android_app* app) : jni(app) { }

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_DECODERS_H
