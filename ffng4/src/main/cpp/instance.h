#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"
#include "files.h"

#include <android_native_app_glue.h>

class Graphics;
class Decoders;
class AudioStream;

struct saved_state {
};

struct Instance {
    android_app* app;

    std::unique_ptr<Graphics> graphics;
    std::unique_ptr<AudioStream> audio;
    std::unique_ptr<Decoders> decoders;

    struct saved_state state;
    bool live;

    Instance(android_app* _app);

    SystemFile systemFile(const std::string& path) const {
        return {path, app->activity->assetManager};
    }

    UserFile userFile(const std::string& path) const {
        return {path, app->activity->externalDataPath};
    }
};

#endif //FISH_FILLETS_INSTANCE_H
