#include "game/screen.h"

std::shared_ptr<Image> GameScreen::addImage(const std::string &filename, const std::string& name) {
    auto [iterator, _] = m_images.insert_or_assign(name.empty() ? filename : name, std::make_shared<Image>(filename));
    return iterator->second;
}

Image& GameScreen::getImage(const std::string& name) {
    return *m_images.at(name);
}

void GameScreen::setBackground(const std::string &filename) {
    addImage(filename, "background");
}

void GameScreen::setMusic(const std::string& filename) {
    m_music = m_instance.audio().loadMusic(filename);
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image->reload(m_instance);
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
        m_instance.graphics().canvas().setWindowSize(bgImage->width(), bgImage->height());
    } catch(std::out_of_range& e) {
        ::error("Level has no background set.");
    }

    own_load();
    m_loadTime = std::chrono::steady_clock::now();

    // Music is playing with the last shown frame while this thread is busy (loadMusic, reloadImages),
    // so make this the very last thing
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

float GameScreen::timeSinceLoad() {
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_loadTime).count();
}