#include "instance.h"

#include "subsystem/graphics.h"
#include "subsystem/input/key.h"
#include "game/screens/screenmanager.h"
#include "game/structure/gametree.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto& instance = Instance::get(app);
    auto& input = instance.screens().curScreen().input();
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        auto action = AMotionEvent_getAction(event);
        if(action == AMOTION_EVENT_ACTION_DOWN) {
            float sx = AMotionEvent_getX(event, 0);
            float sy = AMotionEvent_getY(event, 0);
            return input.handlePointerDown({sx, sy}) ? 1 : 0;
        } else if(action == AMOTION_EVENT_ACTION_MOVE) {
            float sx = AMotionEvent_getX(event, 0);
            float sy = AMotionEvent_getY(event, 0);
            return input.handlePointerMove({sx, sy}) ? 1 : 0;
        } else if(action == AMOTION_EVENT_ACTION_UP) {
            return input.handlePointerUp() ? 1 : 0;
        }
    } else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY) {
        auto key = Input::AndroidKeymap(AKeyEvent_getKeyCode(event));
        auto action = AKeyEvent_getAction(event);
        if(action == AKEY_EVENT_ACTION_DOWN)
            return input.handleKeyDown(key) ? 1 : 0;
        else if(action == AKEY_EVENT_ACTION_UP)
            return input.handleKeyUp(key) ? 1 : 0;
        else
            return 0;
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
                instance.live = true;
                instance.graphics().activate();
                instance.audio().activate();
                instance.screens().refresh();
                instance.screens().drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            instance.graphics().shutdown();
            instance.audio().shutdown();
            instance.live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            if(!instance.running) {
                instance.screens().resume();
                instance.audio().resume();
                instance.running = true;
            }
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            if(instance.running) {
                instance.screens().pause();
                instance.audio().pause();
                instance.running = false;
            }
            break;
        case APP_CMD_START:
            LOGI("APP_CMD_START");
            break;
        case APP_CMD_PAUSE:
            LOGI("APP_CMD_PAUSE");
            if(instance.live && instance.running) {
                instance.screens().pause();
                instance.audio().pause();
                instance.running = false;
            }
            break;
        case APP_CMD_RESUME:
            LOGI("APP_CMD_RESUME");
            if(instance.live && !instance.running) {
                instance.screens().resume();
                instance.audio().resume();
                instance.running = true;
            }
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
            int events;
            struct android_poll_source* source;

            while(ALooper_pollAll(instance.running ? 0 : -1, nullptr, &events, (void**)&source) >= 0) {
                if(source != nullptr)
                    source->process(app, source);
                if(app->destroyRequested) {
                    LOGD("exiting");
                    return;
                }
            }

            if(instance.running)
                instance.screens().drawFrame();
        } catch(std::exception& e) {
            LOGE("Caught exception: %s", e.what());
            instance.quit();
        }
    }
}