#include "instance.h"

#include "subsystem/graphics.h"
#include "game/statemanager.h"
#include "game/worldmap.h"

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    auto* instance = (struct Instance*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION && AMotionEvent_getAction(event) == AMOTION_EVENT_ACTION_DOWN) {
        if(instance->states->getState() == GameState::WorldMap) {
            auto* screen = dynamic_cast<WorldMap*>(instance->curScreen());
            auto sx = (int)AMotionEvent_getX(event, 0);
            auto sy = (int)AMotionEvent_getY(event, 0);
            auto [cx, cy] = instance->graphics->screen2canvas(sx, sy);
            instance->graphics->maskBuffer()->bind();
            auto mask_color = instance->graphics->maskBuffer()->getPixel(cx, cy);
            if(mask_color == WorldMap::MaskColors::exit) {
                screen->staticFrame(WorldMap::Frames::exit);
                instance->graphics->drawFrame();
                instance->quit();
            } else if(mask_color == WorldMap::MaskColors::options) {
                screen->staticFrame(WorldMap::Frames::options);
                instance->graphics->drawFrame();
            } else if(mask_color == WorldMap::MaskColors::intro) {
                screen->staticFrame(WorldMap::Frames::intro);
                instance->graphics->drawFrame();
            } else if(mask_color == WorldMap::MaskColors::credits) {
                screen->staticFrame(WorldMap::Frames::credits);
                instance->graphics->drawFrame();
            } else {
                screen->staticFrame(WorldMap::Frames::loading);
                instance->graphics->drawFrame();
                instance->states->setState(GameState::TestScreen);
            }
        } else {
            instance->states->setState(GameState::WorldMap);
        }
        return 1;
    }
    return 0;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    auto* instance = (struct Instance*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            /*instance->app->savedState = malloc(sizeof(struct saved_state));
            *((struct saved_state*)instance->app->savedState) = instance->state;
            instance->app->savedStateSize = sizeof(struct saved_state);*/
            break;
        case APP_CMD_INIT_WINDOW:
            if (instance->app->window != nullptr) {
                instance->graphics->activate();
                instance->audio->activate();
                instance->live = true;
                instance->curScreen()->load();
                instance->graphics->drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            instance->graphics->shutdown();
            instance->audio->shutdown();
            instance->live = false;
            break;
        case APP_CMD_GAINED_FOCUS:
            instance->live = true;
            break;
        case APP_CMD_LOST_FOCUS:
            instance->live = false;
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

    /*if (app->savedState != nullptr)
        instance->state = *(struct saved_state*)app->savedState;*/

    while(true) {
        try {
            int ident;
            int events;
            struct android_poll_source *source;

            while ((ident = ALooper_pollAll(instance->live ? 0 : -1, nullptr, &events,
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

            if (instance->live)
                instance->graphics->drawFrame();
        } catch(std::exception& e) {
            LOGE("Caught exception: %s", e.what());
            instance->quit();
        }
    }
}