#ifndef FISH_FILLETS_GAME_SCREEN_H
#define FISH_FILLETS_GAME_SCREEN_H

#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/script.h"
#include "subsystem/input.h"

class GameScreen {
protected:
    Instance& m_instance;
    std::chrono::steady_clock::time_point m_relStartTime;
    std::chrono::steady_clock::time_point m_pauseTime;
    bool m_running;

private:
    std::map<std::string, PNGImage> m_images;

public:
    virtual ~GameScreen() { }

    virtual IInputSink& input() = 0;

    void start();
    void resize();
    void pause();
    void resume();
    void update();
    void draw(const DrawTarget& target);
    bool pointer(FCoords coords) { return own_pointer(coords); }
    bool keypress(Key key) { return own_key(key); }

    float timeAlive();

protected:
    GameScreen(Instance& instance);

    Image* addImage(const std::string& path, const std::string& name = "");
    Image* replaceImage(const std::string& name, const std::string& path);
    Image* getImage(const std::string& name);

    virtual void own_start() { }
    virtual void own_pause() { }
    virtual void own_resume() { }
    virtual void own_resize();
    virtual void own_update() { }
    virtual void own_draw(const DrawTarget& target) = 0;
    virtual bool own_pointer(FCoords coords) { return false; }
    virtual bool own_key(Key key) { return false; }
};

#endif //FISH_FILLETS_GAME_SCREEN_H
