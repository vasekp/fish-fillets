#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/rng.h"
#include "game/screens.h"
#include "game/levels.h"

Instance::Instance(android_app* androidApp) : m_app(androidApp), m_jni(m_app), live(false), quit_request(false) {
    m_files = std::make_unique<Files>(*this);
    m_graphics = std::make_unique<Graphics>(*this);
    m_audio = std::make_unique<Audio>(*this);
    m_levels = std::make_unique<Levels>(*this);
    m_screens = std::make_unique<Screens>(*this);
}

Instance& Instance::get(android_app* app) {
    return *static_cast<Instance*>(app->userData);
}

GameScreen& Instance::curScreen() {
    return screens().curScreen();
}

void Instance::quit() {
    live = false;
    audio().clear();
    ANativeActivity_finish(m_app->activity);
}