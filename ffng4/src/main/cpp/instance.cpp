#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/input.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/rng.h"
#include "subsystem/input.h"
#include "game/screens/screenmanager.h"
#include "game/structure/gametree.h"

#include <android/configuration.h>

Instance::Instance(android_app* androidApp) :
        m_app(androidApp),
        m_jni(m_app),
        live(false),
        running(false)
{
    m_files = std::make_unique<Files>(*this);
    m_graphics = std::make_unique<Graphics>(*this);
    m_audio = std::make_unique<Audio>(*this);
    m_levels = std::make_unique<GameTree>(*this);
    m_screens = std::make_unique<ScreenManager>(*this);
    m_input = std::make_unique<AndroidInput>(*this);
}

Instance& Instance::get(android_app* app) {
    return *static_cast<Instance*>(app->userData);
}

void Instance::quit() {
    running = false;
    audio().clear();
    ANativeActivity_finish(m_app->activity);
}