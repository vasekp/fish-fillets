#include "screen.h"

GameScreen::GameScreen(Instance &instance, unsigned int width, unsigned int height) :
    m_instance(instance),
    m_width(width),
    m_height(height)
{ }

std::shared_ptr<Image> GameScreen::addImage(const std::string &filename, const std::string& name) {
    auto [iterator, _] = m_images.insert_or_assign(name.empty() ? filename : name, std::make_shared<Image>(filename));
    return iterator->second;
}

Image& GameScreen::getImage(const std::string& name) {
    return *m_images.at(name);
}

void GameScreen::setSize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image->reload(m_instance);
}

void GameScreen::start() {
    LOGD("screen: start");
    own_start();
    load();
}

void GameScreen::load() {
    LOGD("screen: load");
    reloadImages();

    m_instance.graphics().canvas().setWindowSize(m_width, m_height);

    own_load();
    m_loadTime = std::chrono::steady_clock::now();
}

float GameScreen::timeSinceLoad() {
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_loadTime).count();
}