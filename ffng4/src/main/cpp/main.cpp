#include "instance.h"

#include "subsystem/graphics.h"
#include "subsystem/input.h"
#include "game/structure/gametree.h"
#include "game/screens/screenmanager.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto& instance = Instance::get(app);
    return instance.input().processEvent(event);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto& instance = Instance::get(app);
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            Log::debug("APP_CMD_SAVE_STATE");
            /*instance.app()->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance.app()->savedState) = instance.state;
            instance.app()->savedStateSize = sizeof(struct saved_state);*/
            break;
        case APP_CMD_INIT_WINDOW:
            Log::debug("APP_CMD_INIT_WINDOW");
            if (instance.app()->window != nullptr) {
                instance.live = true;
                instance.graphics().activate();
                instance.audio().activate();
                instance.screens().refresh();
                instance.screens().drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            Log::debug("APP_CMD_TERM_WINDOW");
            instance.graphics().shutdown();
            instance.audio().shutdown();
            instance.live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            Log::debug("APP_CMD_GAINED_FOCUS");
            if(!instance.running) {
                instance.screens().resume();
                instance.audio().resume();
                instance.running = true;
            }
            break;
        case APP_CMD_LOST_FOCUS:
            Log::debug("APP_CMD_LOST_FOCUS");
            if(instance.running) {
                instance.screens().pause();
                instance.audio().pause();
                instance.running = false;
            }
            break;
        case APP_CMD_START:
            Log::debug("APP_CMD_START");
            break;
        case APP_CMD_PAUSE:
            Log::debug("APP_CMD_PAUSE");
            if(instance.live && instance.running) {
                instance.screens().pause();
                instance.audio().pause();
                instance.running = false;
            }
            break;
        case APP_CMD_RESUME:
            Log::debug("APP_CMD_RESUME");
            if(instance.live && !instance.running) {
                instance.screens().resume();
                instance.audio().resume();
                instance.running = true;
            }
            break;
        case APP_CMD_STOP:
            Log::debug("APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY:
            Log::debug("APP_CMD_DESTROY");
            break;
        case APP_CMD_WINDOW_RESIZED:
            Log::debug("APP_CMD_WINDOW_RESIZED");
            Log::debug("native window: ", ANativeWindow_getWidth(app->window), " ", ANativeWindow_getHeight(app->window));
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            Log::debug("APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            Log::debug("APP_CMD_CONTENT_RECT_CHANGED");
            Log::debug("rect: ", app->contentRect.left, " ", app->contentRect.top, " ", app->contentRect.right, " ", app->contentRect.bottom);
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    Instance instance(app);

    app->userData = &instance;
    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    /*if (app->savedState != nullptr)
        instance.state = *(struct saved_state*)app->savedState;*/
    instance.screens().startMode(ScreenManager::Mode::WorldMap);

    while(true) {
        try {
            int events;
            struct android_poll_source* source;

            while(ALooper_pollAll(instance.running ? 0 : -1, nullptr, &events, (void**)&source) >= 0) {
                if(source != nullptr)
                    source->process(app, source);
                if(app->destroyRequested) {
                    Log::info("exiting");
                    return;
                }
            }
            instance.input().ping();

            if(instance.running)
                instance.screens().drawFrame();
        } catch(std::exception& e) {
            Log::error("Caught exception: %s", e.what());
            instance.quit();
        }
    }
}
