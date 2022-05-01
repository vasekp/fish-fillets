#include "game/screen.h"

void GameScreen::addImage(const std::string& name, const std::string &filename) {
    m_images.insert_or_assign(name, Image(filename));
}

void GameScreen::setBackground(const std::string &filename) {
    addImage("background", filename);
}

void GameScreen::setMusic(const std::string& filename) {
    m_music = std::make_shared<AudioSource>(m_instance->audio->loadMusic(filename));
}

const Image* GameScreen::getImage(const std::string& name) {
    return &m_images.at(name);
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image.reload(m_instance);
}

void GameScreen::start() {
    LOGD("screen: start");
    m_music->rewind();
    load();
}

void GameScreen::load() {
    LOGD("screen: load");
    reloadImages();

    try {
        auto& bgImage = m_images.at("background");
        m_instance->graphics->setCanvasSize(bgImage.width(), bgImage.height());
    } catch(std::out_of_range& e) {
        ::error("Level has no background set.");
    }

    own_load();

    // Music is playing with the last shown frame while this thread is busy (loadMusic, reloadImages),
    // so make this the very last thing
    m_instance->audio->clear();
    m_instance->audio->addSource(m_music);
}