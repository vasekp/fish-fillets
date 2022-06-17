#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "platform/instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/input.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/rng.h"
#include "game/screens/screenmanager.h"
#include "game/structure/gametree.h"

template<typename T>
Instance::Instance(T platformInit) :
        live(false),
        running(false)
{
    m_platform = std::make_unique<PlatformInstance>(*this, platformInit);
    m_files = std::make_unique<Files>(*this);
    m_graphics = std::make_unique<Graphics>(*this);
    m_audio = std::make_unique<Audio>(*this);
    m_levels = std::make_unique<GameTree>(*this);
    m_screens = std::make_unique<ScreenManager>(*this);
    m_input = std::make_unique<PlatformInput>(*this);
    m_rng = std::make_unique<RNG>();
}

template Instance::Instance(PlatformData);

template<typename T>
Instance& Instance::get(T) {
    throw std::logic_error("Instance::get called without specialization");
}

void Instance::quit() {
    running = false;
    audio().clear();
    m_platform->quit();
}

Instance::~Instance() = default;
