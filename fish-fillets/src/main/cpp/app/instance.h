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
protected:
    Instance(std::unique_ptr<IFiles>&& files);
    ~Instance();

    void init();

public:
    auto& files() { return *m_files; }
    auto& graphics() { return *m_graphics; }
    auto& audio() { return *m_audio; }
    auto& rng() { return *m_rng; }
    auto& screens() { return *m_screens; }
    auto& levels() { return *m_levels; }
    auto& persist() { return *m_persist; }

    virtual IInputSource& inputSource() = 0;
    IInputSource& inputSourceMasked();
    IInputSink& inputSink();

    virtual std::string lang() = 0;

    void updateAndDraw();
    void run();
    void pause();
    void quit();
    bool running() { return m_running; };

private:
    std::unique_ptr<IFiles> m_files;
    std::unique_ptr<Persist> m_persist;
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<Audio> m_audio;
    std::unique_ptr<GameTree> m_levels;
    std::unique_ptr<ScreenManager> m_screens;
    std::unique_ptr<RNG> m_rng;
    bool m_running;

    virtual void own_run() { }
    virtual void own_pause() { }
    virtual void own_quit() { }
};

#endif //FISH_FILLETS_INSTANCE_H
