#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"
#include "files.h"

#include <android_native_app_glue.h>

struct android_app;

namespace ogl {
    class Display;
    class Framebuffer;
    class Texture;
}

struct Shaders;
class Decoders;

struct saved_state {
};

struct Instance {
    android_app* app;

    std::unique_ptr<ogl::Display> display;
    std::unique_ptr<ogl::Framebuffer> canvas;
    std::unique_ptr<Shaders> shaders;
    std::unique_ptr<ogl::Texture> bg;

    std::unique_ptr<Decoders> decoders;

    struct saved_state state;
    bool live;

    Instance(android_app* _app) : app(_app), live(false) { }

    SystemFile systemFile(const std::string& path) const {
        return {path, app->activity->assetManager};
    }

    UserFile userFile(const std::string& path) const {
        return {path, app->activity->externalDataPath};
    }
};

#endif //FISH_FILLETS_INSTANCE_H
