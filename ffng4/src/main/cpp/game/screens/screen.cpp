#include "screen.h"

GameScreen::GameScreen(Instance &instance) :
    m_instance(instance),
    m_lastDraw(0), m_lastDrawOverlays(0),
    m_running(false)
{ }

Image* GameScreen::addImage(const std::string& filename, const std::string& name) {
    const std::string key = name.empty() ? filename : name;
    if(m_images.contains(key))
        return &m_images.at(key);
    else {
        auto[iterator, _] = m_images.insert({key, Image{m_instance.files().system(filename)}});
        return &iterator->second;
    }
}

Image* GameScreen::replaceImage(const std::string& name, const std::string& filename) {
    auto[iterator, _] = m_images.insert_or_assign(name, Image{m_instance.files().system(filename), m_instance});
    return &iterator->second;
}

Image* GameScreen::getImage(const std::string& name) {
    return &m_images.at(name);
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image.reload(m_instance);
}

void GameScreen::start() {
    Log::debug("screen: start");
    own_start();
    m_relStartTime = m_pauseTime = std::chrono::steady_clock::now();
}

void GameScreen::refresh() {
    Log::debug("screen: refresh");
    reloadImages();
    own_setsize();
    own_refresh();
}

void GameScreen::pause() {
    Log::debug("screen: pause");
    if(!m_running)
        throw std::logic_error("pause() while not running");
    m_pauseTime = std::chrono::steady_clock::now();
    m_running = false;
    own_pause();
}

void GameScreen::resume() {
    Log::debug("screen: resume");
    if(m_running)
        throw std::logic_error("resume() while already running");
    m_relStartTime = std::chrono::steady_clock::now() - (m_pauseTime - m_relStartTime);
    m_running = true;
    own_resume();
}

float GameScreen::timeAlive() {
    return std::chrono::duration<float>(std::chrono::steady_clock::now() - m_relStartTime).count();
}

void GameScreen::draw(const DrawTarget& target) {
    target.bind();
    auto liveTime = timeAlive();
    own_draw(target, liveTime - m_lastDraw);
    m_lastDraw = liveTime;
}

void GameScreen::drawOverlays(const DrawTarget& target) {
    target.bind();
    auto liveTime = timeAlive();
    own_drawOverlays(target, liveTime - m_lastDrawOverlays, liveTime);
    m_lastDrawOverlays = liveTime;
}