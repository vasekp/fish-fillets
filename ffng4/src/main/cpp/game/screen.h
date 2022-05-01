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

private:
    std::map<std::string, Image> m_images;

public:
    virtual ~GameScreen() { }

    virtual void start();
    virtual void load();
    void draw() { own_draw(); };

protected:
    GameScreen(Instance* instance) : m_instance(instance) { }

    void setBackground(const std::string& filename);
    void setMusic(const std::string& filename);

    void addImage(const std::string& name, const std::string& path);
    const Image* getImage(const std::string& name);

    void reloadImages();
    virtual void own_draw() = 0;
    virtual void own_load() { }
};

#endif //FISH_FILLETS_GAME_SCREEN_H