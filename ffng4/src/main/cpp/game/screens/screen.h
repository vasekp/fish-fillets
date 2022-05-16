#ifndef FISH_FILLETS_GAME_SCREEN_H
#define FISH_FILLETS_GAME_SCREEN_H

#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"
#include "subsystem/input.h"

class GameScreen {
protected:
    Instance& m_instance;
    std::map<std::string, AudioSource> m_sounds;
    std::chrono::steady_clock::time_point m_relStartTime;
    std::chrono::steady_clock::time_point m_pauseTime;
    float m_lastDraw;
    float m_lastDrawOverlays;
    bool m_running;

private:
    std::map<std::string, Image> m_images;

public:
    virtual ~GameScreen() { }

    void start();
    void refresh();
    void pause();
    void resume();
    void draw(const DrawTarget& target);
    void drawOverlays(const DrawTarget& target);
    bool mouse(FCoords coords);
    bool keypress(Key key);

    constexpr static unsigned baseWidth = 640;
    constexpr static unsigned baseHeight = 480;

protected:
    GameScreen(Instance& instance);

    void setSize(unsigned width, unsigned height);
    Image& addImage(const std::string& path, const std::string& name = "");
    Image& getImage(const std::string& name);
    void reloadImages();
    float timeAlive();

    virtual void own_start() { }
    virtual void own_refresh() { }
    virtual void own_pause() { }
    virtual void own_resume() { }
    virtual void own_draw(const DrawTarget& target, float dt) = 0;
    virtual void own_drawOverlays(const DrawTarget& target, float dTime, float absTime) { }
    virtual bool own_mouse(unsigned x, unsigned y) { return false; }
    virtual bool own_key(Key key) { return false; }
};

#endif //FISH_FILLETS_GAME_SCREEN_H