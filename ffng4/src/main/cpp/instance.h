#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"
#include "platform/jni.h"

struct android_app;
class Files;
class Graphics;
class Image;
class Audio;

struct saved_state {
};

struct Instance {
    android_app* app;
    jni::Env jni;

    std::unique_ptr<Files> files;
    std::unique_ptr<Graphics> graphics;
    std::unique_ptr<Audio> audio;

    std::unique_ptr<Image> bg;

    struct saved_state state;
    bool live;

    Instance(android_app*);
};

#endif //FISH_FILLETS_INSTANCE_H
