#ifndef FISH_FILLETS_GAME_SCREEN_H
#define FISH_FILLETS_GAME_SCREEN_H

#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"
#include "subsystem/script.h"

class GameScreen {
protected:
    Instance& m_instance;
    std::map<std::string, std::shared_ptr<AudioSource>> m_sounds;
    std::chrono::steady_clock::time_point m_loadTime;
    unsigned m_width, m_height;

private:
    std::map<std::string, Image> m_images;

public:
    virtual ~GameScreen() { }

    void start();
    void load();
    void draw() { own_draw(); }
    bool mouse(Coords coords) { return own_mouse(coords.x(), coords.y()); }

    constexpr static unsigned baseWidth = 640;
    constexpr static unsigned baseHeight = 480;

protected:
    GameScreen(Instance& instance, unsigned width = baseWidth, unsigned height = baseHeight);

    void setSize(unsigned width, unsigned height);
    Image& addImage(const std::string& path, const std::string& name = "");
    Image& getImage(const std::string& name);
    void reloadImages();
    float timeSinceLoad();

    virtual void own_start() { }
    virtual void own_load() { }
    virtual void own_draw() = 0;
    virtual bool own_mouse(unsigned x, unsigned y) { return false; }
};

#endif //FISH_FILLETS_GAME_SCREEN_H