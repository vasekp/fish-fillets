#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"

struct android_app;
class Files;
class Graphics;
class Image;
class Decoders;
class AudioStream;

struct saved_state {
};

struct Instance {
    android_app* app;

    std::unique_ptr<Files> files;
    std::shared_ptr<Graphics> graphics;
    std::unique_ptr<AudioStream> audio;
    std::unique_ptr<Decoders> decoders;

    std::unique_ptr<Image> bg;

    struct saved_state state;
    bool live;

    Instance(android_app* _app);
};

#endif //FISH_FILLETS_INSTANCE_H
