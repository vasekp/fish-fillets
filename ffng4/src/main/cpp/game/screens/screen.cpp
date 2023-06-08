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
        auto[iterator, _] = m_images.try_emplace(key, m_instance, filename);
        return &iterator->second;
    }
}

Image* GameScreen::replaceImage(const std::string& name, const std::string& filename) {
    auto[iterator, _] = m_images.insert_or_assign(name, PNGImage(m_instance, filename));
    return &iterator->second;
}

Image* GameScreen::getImage(const std::string& name) {
    return &m_images.at(name);
}

void GameScreen::start() {
    Log::debug("screen: start");
    own_start();
    resize();
    m_relStartTime = m_pauseTime = std::chrono::steady_clock::now();
}

void GameScreen::resize() {
    own_resize();
}

void GameScreen::own_resize() {
    m_instance.graphics().setWindowSize(Graphics::baseDim.x(), Graphics::baseDim.y());
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
    return m_running
        ? std::chrono::duration<float>(std::chrono::steady_clock::now() - m_relStartTime).count()
        : 0;
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
    own_drawOverlays(target, liveTime - m_lastDrawOverlays, liveTime); // TODO 3rd argument not needed
    m_lastDrawOverlays = liveTime;
}
