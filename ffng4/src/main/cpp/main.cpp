#include "instance.h"

#include "subsystem/graphics.h"
#include "game/screens/screenmanager.h"
#include "game/screens/worldmap.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto& instance = Instance::get(app);
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION && AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
        auto sx = (int)AMotionEvent_getX(event, 0);
        auto sy = (int)AMotionEvent_getY(event, 0);
        auto canvasCoords = instance.graphics().windowTarget().screen2canvas({sx, sy});
        return instance.screens().mouse(canvasCoords) ? 1 : 0;
    } else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY && AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN) {
        instance.screens().options() = !instance.screens().options();
    }
    return 0;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto& instance = Instance::get(app);
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            LOGI("APP_CMD_SAVE_STATE");
            /*instance.app()->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance.app()->savedState) = instance.state;
            instance.app()->savedStateSize = sizeof(struct saved_state);*/
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            if (instance.app()->window != nullptr) {
                instance.graphics().activate();
                instance.audio().activate();
                instance.screens().refresh();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            instance.graphics().shutdown();
            instance.audio().shutdown();
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            instance.screens().resume();
            instance.audio().resume();
            instance.live = true;
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            if(instance.live) {
                instance.screens().pause();
                instance.audio().pause();
                instance.live = false;
            }
            break;
        case APP_CMD_START:
            LOGI("APP_CMD_START");
            break;
        case APP_CMD_PAUSE:
            LOGI("APP_CMD_PAUSE");
            if(instance.live) {
                instance.screens().pause();
                instance.audio().pause();
                instance.live = false;
            }
            break;
        case APP_CMD_RESUME:
            LOGI("APP_CMD_RESUME");
            break;
        case APP_CMD_STOP:
            LOGI("APP_CMD_STOP");
            break;
        case APP_CMD_DESTROY:
            LOGI("APP_CMD_DESTROY");
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
            int ident;
            int events;
            struct android_poll_source *source;

            while ((ident = ALooper_pollAll(instance.live ? 0 : -1, nullptr, &events,
                                            (void **) &source)) >= 0) {
                if (source != nullptr)
                    source->process(app, source);
                if (ident == LOOPER_ID_USER) {
                    // sensors...
                }
                if (app->destroyRequested) {
                    LOGD("exiting");
                    return;
                }
            }

            if (instance.live)
                instance.screens().drawFrame();
        } catch(std::exception& e) {
            LOGE("Caught exception: %s", e.what());
            instance.quit();
        }
    }
}