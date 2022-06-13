#include "graphics.h"
#include "game/screens/screen.h"
#include "game/screens/screenmanager.h"

void Graphics::activate() {
    Log::debug("graphics: activate");
    m_system = std::make_unique<GraphicsSystem>(m_instance);
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