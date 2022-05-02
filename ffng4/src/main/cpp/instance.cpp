#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "game/statemanager.h"

Instance::Instance(android_app* androidApp) : app(androidApp), jni(app), live(false), quit_request(false) {
    files = std::make_unique<Files>(this);
    graphics = std::make_unique<Graphics>(this);
    audio = std::make_unique<Audio>(this);
    states = std::make_unique<StateManager>(this);
}

GameScreen* Instance::curScreen() {
    return states->curScreen();
}

void Instance::quit() {
    live = false;
    audio->clear();
    ANativeActivity_finish(app->activity);
}