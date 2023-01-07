#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"

class IFiles;
class Graphics;
class Image;
class Audio;
class GameScreen;
class ScreenManager;
class Script;
class GameTree;
class AudioSource;
class IInputProvider;
class RNG;

class Instance {
    std::unique_ptr<IFiles> m_files;
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<Audio> m_audio;
    std::unique_ptr<ScreenManager> m_screens;
    std::unique_ptr<IInputProvider> m_input;
    std::unique_ptr<Script> m_script;
    std::unique_ptr<GameTree> m_levels;
    std::unique_ptr<RNG> m_rng;

public:
    ~Instance();

    auto& files() { return *m_files; }
    auto& graphics() { return *m_graphics; }
    auto& audio() { return *m_audio; }
    auto& rng() { return *m_rng; }
    auto& input() { return *m_input; }
    auto& screens() { return *m_screens; }
    auto& script() { return *m_script; }
    auto& levels() { return *m_levels; }

    bool live;
    bool running;

    virtual void quit();
    virtual void* window() = 0;

protected:
    Instance(std::unique_ptr<IFiles>&& files, std::unique_ptr<IInputProvider>&& input);
};

#endif //FISH_FILLETS_INSTANCE_H
