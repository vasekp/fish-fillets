#ifndef FISH_FILLETS_GAME_SCREEN_H
#define FISH_FILLETS_GAME_SCREEN_H

#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/script.h"
#include "subsystem/input.h"
#include "liveclock.h"

class GameScreen {
public:
    virtual ~GameScreen() { }

    virtual IInputSink& input() = 0;

    void start();
    void resize();
    void pause();
    void resume();
    void update();
    void draw(DrawTarget& target);
    bool pointer(FCoords coords) { return own_pointer(coords); }
    bool keypress(Key key) { return own_key(key); }

    float timeAlive();
    LiveClock::time_point liveTime();

protected:
    Instance& m_instance;
    bool m_running;
    LiveClock m_clock;

    GameScreen(Instance& instance);

    const Image* addImage(const std::string& path, const std::string& name = "", TextureType type = TextureType::image);
    const Image* replaceImage(const std::string& name, const std::string& path);
    const Image* getImage(const std::string& name);

    virtual void own_start() { }
    virtual void own_pause() { }
    virtual void own_resume() { }
    virtual void own_resize();
    virtual void own_update() { }
    virtual void own_draw(DrawTarget& target) = 0;
    virtual bool own_pointer(FCoords coords) { return false; }
    virtual bool own_key(Key key) { return false; }

private:
    std::map<std::string, ImageRef> m_images;
};

#endif //FISH_FILLETS_GAME_SCREEN_H
