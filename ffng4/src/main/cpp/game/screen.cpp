#include "screen.h"

GameScreen::GameScreen(Instance &instance, unsigned int width, unsigned int height) :
    m_instance(instance),
    m_width(width),
    m_height(height),
    m_running(false)
{ }

Image& GameScreen::addImage(const std::string &filename, const std::string& name) {
    auto [iterator, _] = m_images.insert_or_assign(name.empty() ? filename : name, Image(filename));
    return iterator->second;
}

Image& GameScreen::getImage(const std::string& name) {
    return m_images.at(name);
}

void GameScreen::setSize(unsigned int width, unsigned int height) {
    m_width = width;
    m_height = height;
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image.reload(m_instance);
}

void GameScreen::start() {
    LOGD("screen: start");
    own_start();
    m_relStartTime = m_pauseTime = std::chrono::steady_clock::now();
}

void GameScreen::refresh() {
    LOGD("screen: refresh");
    reloadImages();

    m_instance.graphics().canvas().setWindowSize(m_width, m_height);

    own_refresh();
}

void GameScreen::pause() {
    LOGD("screen: pause");
    if(!m_running)
        throw std::logic_error("pause() while not running");
    m_pauseTime = std::chrono::steady_clock::now();
    m_running = false;
    own_pause();
}

void GameScreen::resume() {
    LOGD("screen: resume");
    if(m_running)
        throw std::logic_error("resume() while already running");
    m_relStartTime = std::chrono::steady_clock::now() - (m_pauseTime - m_relStartTime);
    m_running = true;
    own_resume();
}

float GameScreen::timeAlive() {
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_relStartTime).count();
}