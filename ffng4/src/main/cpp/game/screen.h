#ifndef FISH_FILLETS_GAME_SCREEN_H
#define FISH_FILLETS_GAME_SCREEN_H

#include "instance.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/files.h"

class GameScreen {
protected:
    Instance* m_instance;
    std::shared_ptr<AudioSource> m_music;
    std::map<std::string, AudioSource> m_sounds;
    std::chrono::steady_clock::time_point m_loadTime;

private:
    std::map<std::string, std::unique_ptr<Image>> m_images;

public:
    virtual ~GameScreen() { }

    void start();
    void load();
    void draw() { own_draw(); }
    bool mouse(unsigned x, unsigned y) { return own_mouse(x, y); }

protected:
    GameScreen(Instance* instance) : m_instance(instance) { }

    const Image* addImage(const std::string& name, const std::string& path);
    const Image* getImage(const std::string& name);
    void setBackground(const std::string& filename);
    void setMusic(const std::string& filename);
    void reloadImages();
    float timeSinceLoad();

    virtual void own_draw() = 0;
    virtual void own_load() { }
    virtual bool own_mouse(unsigned x, unsigned y) { return false; }
};

#endif //FISH_FILLETS_GAME_SCREEN_H