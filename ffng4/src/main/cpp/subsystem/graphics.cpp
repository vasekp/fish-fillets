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
    m_reserve = reserve;
    m_windowDim = { width, height };
    recalc();
}

void Graphics::notifyDisplayResize() {
    recalc();
    m_system->resizeBuffers();
}

void Graphics::recalc() {
    FCoords displayDim = {display().width(), display().height()};
    FCoords displayDimReduced = displayDim - m_reserve;
    {
        float scale = std::min(displayDim.fx() / 640, displayDim.fy() / 480); // TODO constexpr
        m_coords[base] = {(displayDim - scale * m_windowDim) / 2.f, scale};
    }
    m_coords[reduced] = {m_reserve, 1};
    {
        float scale = std::min(displayDimReduced.fx() / m_windowDim.fx(), displayDimReduced.fy() / m_windowDim.fy());
        m_coords[window] = {(displayDimReduced - scale * m_windowDim) / 2.f + m_reserve, scale};
    }
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
    auto it = std::find(m_images.begin(), m_images.end(), image);
    if(it != m_images.end())
        m_images.erase(it);
}