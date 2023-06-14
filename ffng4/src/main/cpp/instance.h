#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"

class IFiles;
class Graphics;
class Image;
class Audio;
class GameScreen;
class ScreenManager;
class GameTree;
class AudioSource;
class IInputSource;
class IInputSink;
class Persist;
class RNG;

class Instance {
    std::unique_ptr<IFiles> m_files;
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<Audio> m_audio;
    std::unique_ptr<ScreenManager> m_screens;
    std::unique_ptr<GameTree> m_levels;
    std::unique_ptr<Persist> m_persist;
    std::unique_ptr<RNG> m_rng;

protected:
    Instance();
    void init();
    ~Instance();

public:
    virtual IFiles& files() = 0;
    auto& graphics() { return *m_graphics; }
    auto& audio() { return *m_audio; }
    auto& rng() { return *m_rng; }
    auto& screens() { return *m_screens; }
    auto& levels() { return *m_levels; }
    auto& persist() { return *m_persist; }

    virtual IInputSource& inputSource() = 0;
    IInputSource& inputSourceMasked();
    IInputSink& inputSink();

    bool running = false;

    virtual void quit();
    virtual void* window() = 0;
};

#endif //FISH_FILLETS_INSTANCE_H
