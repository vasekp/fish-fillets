#include "common.h"
#include "assets.h"
#include "display.h"

/*static void draw_frame(struct Instance* instance) {
    if (instance->display == nullptr)
        return;

    // Just fill the screen with a color.
    glClearColor(instance->state.x/(float)instance->width, instance->state.angle,
                 instance->state.y/(float)instance->height, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(instance->display, instance->surface);
}*/

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto* instance = (struct Instance*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        instance->animating = true;
        instance->state.x = AMotionEvent_getX(event, 0);
        instance->state.y = AMotionEvent_getY(event, 0);
        return 1;
    }
    return 0;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto* instance = (struct Instance*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            instance->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance->app->savedState) = instance->state;
            instance->app->savedStateSize = sizeof(struct saved_state);
            break;
        case APP_CMD_INIT_WINDOW:
            if (instance->app->window != nullptr) {
                instance->display = std::make_unique<Display>(instance->app->window);
//                draw_frame(instance);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->display.reset();
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            instance->animating = false;
//            draw_frame(instance);
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    struct Instance instance{app};

    app->userData = &instance;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    instance.app = app;

    if (app->savedState != nullptr)
        instance.state = *(struct saved_state*)app->savedState;

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(instance.animating ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {
            if (source != nullptr)
                source->process(app, source);
            if (ident == LOOPER_ID_USER) {
                // sensors...
            }
            if (app->destroyRequested != 0)
                return;
        }

        if (instance.animating) {
            instance.state.angle += .01f;
            if (instance.state.angle > 1)
                instance.state.angle = 0;
//            draw_frame(&instance);
        }
    }
}