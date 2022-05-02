#include "instance.h"

#include "game/worldmap.h"
#include "game/testscreen.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto* instance = (struct Instance*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION && AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
        /*if(instance->screens.size() == 1) {
            LOGD("Load start");
            auto start = std::chrono::steady_clock::now();
            dynamic_cast<WorldMap*>(instance->screen())->prep_loading();
            instance->graphics->drawFrame();
            instance->screens.push_back(std::make_unique<TestScreen>(instance,
                 "images/start/prvni-p.png",
                 "images/start/prvni-w.png",
                 "music/rybky04.ogg"));
            instance->screen()->start();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            LOGD("Load finished, duration = %f s", diff.count());
        } else {
            LOGD("Resume start");
            auto start = std::chrono::steady_clock::now();
            instance->screens.pop_back();
            instance->screen()->start();
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            LOGD("Resume finished, duration = %f s", diff.count());
        }*/
        auto sx = (int)AMotionEvent_getX(event, 0);
        auto sy = (int)AMotionEvent_getY(event, 0);
        auto [cx, cy] = instance->graphics->screen2canvas(sx, sy);
        LOGD("%d %d", cx, cy);
        instance->graphics->readBuffer()->bind();
        LOGD("%08X", instance->graphics->readBuffer()->getPixel(cx, cy).rgba());
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
                instance->graphics->activate();
                instance->audio->activate();
                instance->live = true;
                instance->screen()->load();
                instance->graphics->drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->graphics->shutdown();
            instance->audio->shutdown();
            instance->live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            break;
        case APP_CMD_LOST_FOCUS:
            instance->live = false;
            instance->graphics->drawFrame();
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    auto instance = std::make_unique<Instance>(app);

    app->userData = instance.get();
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;
    instance->app = app;

    if (app->savedState != nullptr)
        instance->state = *(struct saved_state*)app->savedState;

    instance->screens.push_back(std::make_unique<WorldMap>(instance.get()));

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident=ALooper_pollAll(instance->live ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {
            if (source != nullptr)
                source->process(app, source);
            if (ident == LOOPER_ID_USER) {
                // sensors...
            }
            if (app->destroyRequested != 0)
                return;
        }

        if (instance->live)
            instance->graphics->drawFrame();
    }
}