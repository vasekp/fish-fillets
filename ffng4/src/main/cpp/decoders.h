#ifndef FISH_FILLETS_DECODERS_H
#define FISH_FILLETS_DECODERS_H

#include "common.h"
#include "ndk.h"

#include <android_native_app_glue.h>
#include <jni.h>

namespace ogl {
    class Texture;
}

class Decoders {
    android_app* app;
    ndk::JNIEnv jni;
    jclass clazz;
    std::map<std::string, jmethodID> methods;

public:
    Decoders(android_app* app_);
    Decoders(const Decoders&) = delete;
    Decoders& operator=(const Decoders&) = delete;
    ~Decoders();

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_DECODERS_H
