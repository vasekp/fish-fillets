#include "gamescreen.h"

GameScreen::GameScreen(Instance &instance) :
    m_instance(instance),
    m_running(false)
{ }

const Image* GameScreen::addImage(const std::string& filename, const std::string& name, TextureType type) {
    const std::string key = name.empty() ? filename : name;
    if(m_images.contains(key))
        return &m_images.at(key);
    else {
        auto[iterator, _] = m_images.try_emplace(key, m_instance, filename, type);
        return &iterator->second;
    }
}

const Image* GameScreen::replaceImage(const std::string& name, const std::string& filename) {
    auto[iterator, _] = m_images.insert_or_assign(name, PNGImage(m_instance, filename));
    return &iterator->second;
}

const Image* GameScreen::getImage(const std::string& name) {
    return &m_images.at(name);
}

void GameScreen::start() {
    Log::debug<Log::lifecycle>("screen: start");
    own_start();
    resize();
    m_relStartTime = m_pauseTime = std::chrono::steady_clock::now();
}

void GameScreen::resize() {
    own_resize();
}

void GameScreen::own_resize() {
    m_instance.graphics().setWindowSize(Graphics::baseDim);
}

void GameScreen::pause() {
    if(!m_running)
        return;
    Log::debug<Log::lifecycle>("screen: pause");
    m_pauseTime = std::chrono::steady_clock::now();
    m_running = false;
    own_pause();
}

void GameScreen::resume() {
    if(m_running)
        return;
    Log::debug<Log::lifecycle>("screen: resume");
    m_relStartTime = std::chrono::steady_clock::now() - (m_pauseTime - m_relStartTime);
    m_running = true;
    own_resume();
}

float GameScreen::timeAlive() {
    return m_running
        ? std::chrono::duration<float>(std::chrono::steady_clock::now() - m_relStartTime).count()
        : 0;
}

void GameScreen::update() {
    own_update();
}

void GameScreen::draw(DrawTarget& target) {
    own_draw(target);
}
