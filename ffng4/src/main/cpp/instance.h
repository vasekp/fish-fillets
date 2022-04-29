#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"

#include <android_native_app_glue.h>

#include "ndk.h"
#include "files.h"

struct android_app;
namespace ogl {
    struct Display;
    struct Image;
}

struct saved_state {
    float angle;
    float x;
    float y;
};

struct Instance {
    android_app* app;
    ndk::JNIEnv jni;

    std::unique_ptr<ogl::Display> display;
    std::unique_ptr<ogl::Image> bg;

    struct saved_state state;
    bool animating;

    Instance(android_app* _app) :
            app(_app),
            jni(app->activity->vm)
    { }

    SystemFile systemFile(const std::string& path) {
        return {path, app->activity->assetManager};
    }

    UserFile userFile(const std::string& path) {
        return {path, app->activity->externalDataPath};
    }

    ogl::Image loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_INSTANCE_H
