#include "instance.h"

// All the platform headers needed here as their unique_ptr<>s are instantiated.
// Can not be incomplete at that point.

#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/input.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/rng.h"
#include "game/screens/screenmanager.h"
#include "game/structure/gametree.h"
#include "subsystem/persist.h"

Instance::Instance(std::unique_ptr<IFiles>&& files) :
    m_files(std::move(files)),
    m_persist(std::make_unique<Persist>(*this)), // uses Files in destructor
    m_graphics(std::make_unique<Graphics>(*this)),
    m_audio(std::make_unique<Audio>(*this)), // uses Persist in constructor for volume settings
    m_levels(std::make_unique<GameTree>(*this)),
    m_screens(std::make_unique<ScreenManager>(*this)), // uses Persist in destructor for playtime
    m_rng(std::make_unique<RNG>())
{ }

Instance::~Instance() = default;

IInputSink& Instance::inputSink() {
    return screens().input();
}

IInputSource& Instance::inputSourceMasked() {
    return screens().inputSourceMasked();
}

void Instance::init() {
    if(persist().get("subtitles", "undef"s) == "undef") {
        auto sysLang = lang();
        Log::info<Log::platform>("Lang empty, system: ", sysLang);
        persist().set("subtitles", sysLang == "cs" || sysLang == "sk" ? "cs"s : "en"s);
    }

    bool playIntro = !persist().get<int>("intro", 0);
    screens().startMode(playIntro ? ScreenManager::Mode::Intro : ScreenManager::Mode::WorldMap);
    if(playIntro)
        persist().set("intro", 1);
}

void Instance::updateAndDraw() {
    inputSource().ping();
    screens().updateAll();
    screens().drawFrame();
}

void Instance::quit() {
    running = false;
    audio().clear();
    own_quit();
}
