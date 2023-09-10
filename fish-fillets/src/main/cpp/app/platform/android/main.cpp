#include "ainstance.h"
#include "subsystem/graphics.h"
#include "subsystem/graphics/graphicsbackend.h"
#include "subsystem/input.h"
#include "game/screens/screenmanager.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto& instance = AndroidInstance::get(app);
    return instance.inputSource().processEvent(event);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto& instance = AndroidInstance::get(app);
    switch(cmd) {
        case APP_CMD_SAVE_STATE:
            Log::debug<Log::platform>("APP_CMD_SAVE_STATE");
            // FIXME: there is nothing to save across restarts, unless we can do a full Lua state dump
            break;
        case APP_CMD_INIT_WINDOW:
            Log::debug<Log::platform>("APP_CMD_INIT_WINDOW");
            if(app->window != nullptr) {
                instance.live = true;
                auto display = ogl::Display(app->window);
                instance.graphics().activate(std::move(display));
                instance.oboe().open();
                instance.screens().resize();
                instance.screens().drawFrame();
            }
            if(instance.live)
                instance.run();
            break;
        case APP_CMD_TERM_WINDOW:
            Log::debug<Log::platform>("APP_CMD_TERM_WINDOW");
            instance.graphics().shutdown();
            instance.oboe().close();
            instance.live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
        case APP_CMD_RESUME:
            Log::debug<Log::platform>(cmd == APP_CMD_RESUME ? "APP_CMD_RESUME" : "APP_CMD_GAINED_FOCUS");
            if(instance.live)
                instance.run();
            break;
        case APP_CMD_LOST_FOCUS:
        case APP_CMD_PAUSE:
            Log::debug<Log::platform>(cmd == APP_CMD_PAUSE ? "APP_CMD_PAUSE" : "APP_CMD_LOST_FOCUS");
            if(instance.live)
                instance.pause();
            break;
        case APP_CMD_START:
            Log::debug<Log::platform>("APP_CMD_START");
            break;
        case APP_CMD_STOP:
            Log::debug<Log::platform>("APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY:
            Log::debug<Log::platform>("APP_CMD_DESTROY");
            break;
        case APP_CMD_WINDOW_RESIZED:
            Log::debug<Log::platform>("APP_CMD_WINDOW_RESIZED");
            Log::debug<Log::platform>("native window: ", ANativeWindow_getWidth(app->window), " ", ANativeWindow_getHeight(app->window));
            instance.graphics().notifyResize({(unsigned)ANativeWindow_getWidth(app->window), (unsigned)ANativeWindow_getHeight(app->window)});
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            Log::debug<Log::platform>("APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            Log::debug<Log::platform>("APP_CMD_CONTENT_RECT_CHANGED");
            Log::debug<Log::platform>("rect: ", app->contentRect.left, " ", app->contentRect.top, " ", app->contentRect.right, " ", app->contentRect.bottom);
            break;
        default:
            break;
    }
}

void android_main(struct android_app* app) {
    AndroidInstance instance(app);

    app->onAppCmd = handle_cmd;
    app->onInputEvent = handle_input;

    while(true) {
        try {
            int events;
            struct android_poll_source* source;

            while(ALooper_pollAll(instance.running() ? 0 : -1, nullptr, &events, (void**)&source) >= 0) {
                if(source != nullptr)
                    source->process(app, source);
                if(app->destroyRequested) {
                    Log::info<Log::lifecycle>("exiting");
                    return;
                }
            }

            if(instance.running())
                instance.updateAndDraw();
        } catch(std::exception& e) {
            Log::error("Caught exception: %s", e.what());
            instance.quit();
        }
    }
}
