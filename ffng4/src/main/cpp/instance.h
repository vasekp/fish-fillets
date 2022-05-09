#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"
#include "platform/jni.h"

struct android_app;
class Files;
class Graphics;
class Image;
class Audio;
class GameScreen;
class StateManager;
class Script;
class Levels;
class AudioSource;

/*struct saved_state {
};*/

class Instance {
    android_app* m_app;
    jni::Env m_jni;

    std::unique_ptr<Files> m_files;
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<Audio> m_audio;
    std::unique_ptr<StateManager> m_states;
    std::unique_ptr<Script> m_script;
    std::unique_ptr<Levels> m_levels;
    std::vector<AudioSource> m_sounds_preload;

    //struct saved_state m_state;

public:
    Instance(android_app*);
    static Instance& get(android_app*);

    auto* app() { return m_app; }
    auto& jni() { return m_jni; }
    auto& files() { return *m_files; }
    auto& graphics() { return *m_graphics; }
    auto& audio() { return *m_audio; }
    auto& states() { return *m_states; }
    auto& script() { return *m_script; }
    auto& levels() { return *m_levels; }

    bool live;
    bool quit_request;

    GameScreen& curScreen();

    void quit();
};

#endif //FISH_FILLETS_INSTANCE_H