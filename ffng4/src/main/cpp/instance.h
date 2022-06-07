#ifndef FISH_FILLETS_INSTANCE_H
#define FISH_FILLETS_INSTANCE_H

#include "common.h"
//#include "api/jni.h" // XXX

//struct android_app; // XXX
class Files;
class Graphics;
class Image;
class Audio;
class GameScreen;
class ScreenManager;
class Script;
class GameTree;
class AudioSource;
class AndroidInput;
class RNG;

/*struct saved_state {
};*/

class Instance {
//    android_app* m_app; // XXX
//    jni::Env m_jni;

    std::unique_ptr<Files> m_files;
    std::unique_ptr<Graphics> m_graphics;
    std::unique_ptr<Audio> m_audio;
    std::unique_ptr<ScreenManager> m_screens;
    std::unique_ptr<AndroidInput> m_input;
    std::unique_ptr<Script> m_script;
    std::unique_ptr<GameTree> m_levels;
    std::unique_ptr<RNG> m_rng;

    //struct saved_state m_screens;

public:
//    Instance(android_app*); // XXX
//    static Instance& get(android_app*);
//    ~Instance();
//
//    auto* app() { return m_app; }
//    auto& jni() { return m_jni; }
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

    void quit();
};

#endif //FISH_FILLETS_INSTANCE_H
