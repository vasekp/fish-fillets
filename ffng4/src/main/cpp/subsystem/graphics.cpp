#include "graphics.h"
#include "game/screens/screen.h"
#include "game/screens/screenmanager.h"

void Graphics::activate() {
    Log::debug("graphics: activate");
    m_system = std::make_unique<GraphicsSystem>(m_instance);
    for(auto* image : m_images)
        image->renderTexture();
}

void Graphics::shutdown() {
    Log::debug("graphics: shutdown");
    m_system.reset();
}

void Graphics::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    if(!m_system)
        Log::fatal("setWindowSize() called before activate()");
    m_system->setWindowSize(width, height, reserve);
}

void Graphics::setMask(const ogl::Texture& texture) {
    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(Shaders::texImage_gl);
}

void Graphics::setMask(const Image* image) {
    setMask(image->texture());
}

void Graphics::regImage(Image* image) {
    m_images.push_back(image);
    if(m_system)
        image->renderTexture();
}

void Graphics::regImageMove(Image* from, Image* to) noexcept {
    *std::find(m_images.begin(), m_images.end(), from) = to;
}

void Graphics::unregImage(Image* image) noexcept {
    m_images.erase(std::find(m_images.begin(), m_images.end(), image));
}