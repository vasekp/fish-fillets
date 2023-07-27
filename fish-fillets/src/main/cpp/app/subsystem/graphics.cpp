#include "graphics.h"
#include "game/screens/screenmanager.h"

void Graphics::activate(std::unique_ptr<GraphicsSystem>&& system) {
    Log::debug<Log::graphics>("graphics: activate");
    m_system = std::move(system);
    for(auto* image : m_images)
        image->render();
}

void Graphics::shutdown() {
    Log::debug<Log::graphics>("graphics: shutdown");
    m_system.reset();
}

void Graphics::setWindowSize(FCoords size) {
    m_windowDim = size;
    m_windowShift = {};
    if(m_system)
        recalc();
}

void Graphics::setWindowShift(FCoords shift) {
    m_windowShift = shift;
    m_coords[window] = m_coords[window0];
    m_coords[window].origin += m_windowShift;
}

void Graphics::setViewport(FCoords origin, FCoords size) {
    Log::debug<Log::graphics>("viewport origin ", origin, " size ", size);
    m_system->setViewport(origin.round(), size.round());
    recalc();
    m_system->resizeBuffers();
    m_instance.screens().resize();
}

void Graphics::recalc() {
    FCoords displayDim = m_system->display().size();
    float scale0 = std::min(displayDim.fx() / baseDim.fx(), displayDim.fy() / baseDim.fy());
    m_coords[base] = {(displayDim - scale0 * baseDim) / 2.f, scale0, baseDim};
    float stripSize = 64 * scale0; // TODO
    float scale1 = std::min((displayDim.fx() - stripSize) / m_windowDim.fx(), displayDim.fy() / m_windowDim.fy());
    float scale2 = std::min(displayDim.fx() / m_windowDim.fx(), (displayDim.fy() - stripSize) / m_windowDim.fy());
    float scale = std::max(scale1, scale2);
    bool vert = scale1 > scale2;
    FCoords center = vert ? FCoords{stripSize / 2.f, displayDim.fy() / 2.f} : FCoords{displayDim.fx() / 2.f, stripSize / 2.f};
    FCoords principal = vert ? FCoords{0.f, 1.f} : FCoords{1.f, 0.f};
    m_coords[buttons] = {center, scale0, {}, principal};
    FCoords reduceBase = vert ? FCoords{stripSize, 0.f} : FCoords{0.f, stripSize};
    FCoords reduceDim = displayDim - reduceBase;
    float scale3 = std::min(reduceDim.fx() / baseDim.fx(), reduceDim.fy() / baseDim.fy());
    m_coords[reduced] = { reduceBase + (reduceDim - scale3 * baseDim) / 2.f, scale3, reduceDim};
    m_coords[window0] = {(displayDim - reduceBase - scale * m_windowDim) / 2.f + reduceBase, scale, m_windowDim};
    m_coords[window] = m_coords[window0];
    m_coords[window].origin += m_windowShift;
}

void Graphics::regImage(Image* image) {
    m_images.push_back(image);
    if(m_system)
        image->render();
}

void Graphics::regImageMove(Image* from, Image* to) noexcept {
    *std::find(m_images.begin(), m_images.end(), from) = to;
}

void Graphics::unregImage(Image* image) noexcept {
    auto it = std::find(m_images.begin(), m_images.end(), image);
    if(it != m_images.end())
        m_images.erase(it);
}