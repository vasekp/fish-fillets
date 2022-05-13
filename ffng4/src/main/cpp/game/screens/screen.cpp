#include "screen.h"

GameScreen::GameScreen(Instance &instance) :
    m_instance(instance),
    m_lastDraw(0),
    m_running(false)
{ }

Image& GameScreen::addImage(const std::string& filename, const std::string& name) {
    const std::string key = name.empty() ? filename : name;
    if(m_images.contains(key))
        return m_images.at(key);
    else {
        auto[iterator, _] = m_images.insert({key, {m_instance.files().system(filename)}});
        return iterator->second;
    }
}

Image& GameScreen::getImage(const std::string& name) {
    return m_images.at(name);
}

void GameScreen::setSize(unsigned int width, unsigned int height) {
    m_instance.graphics().setWindowSize(width, height);
}

void GameScreen::reloadImages() {
    for(auto& [_, image] : m_images)
        image.reload(m_instance);
}

void GameScreen::start() {
    LOGD("screen: start");
    m_instance.graphics().setWindowSize(baseWidth, baseHeight);
    own_start();
    m_relStartTime = m_pauseTime = std::chrono::steady_clock::now();
}

void GameScreen::refresh() {
    LOGD("screen: refresh");
    reloadImages();
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

void GameScreen::draw(const DrawTarget& target) {
    target.bind();
    auto liveTime = timeAlive();
    own_draw(target, liveTime - m_lastDraw);
    m_lastDraw = liveTime;
}

bool GameScreen::mouse(Coords coords) {
    return own_mouse(coords.x(), coords.y());
}

bool GameScreen::keypress(Key key) {
    return own_key(key);
}