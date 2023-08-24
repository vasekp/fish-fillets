#include "graphics.h"
#include "graphics/graphicsbackend.h"
#include "game/screens/screenmanager.h"

Graphics::Graphics(Instance& instance) : m_instance(instance) { }

Graphics::~Graphics() = default;

void Graphics::activate(BACKEND::Display&& display) {
    Log::debug<Log::graphics>("graphics: activate");
    m_backend = std::make_unique<GraphicsBackend>(m_instance, std::move(display));
    for(const auto& image : m_images)
        image->render(m_instance);
}

void Graphics::shutdown() {
    Log::debug<Log::graphics>("graphics: shutdown");
    m_backend.reset();
}

void Graphics::setWindowSize(FCoords size) {
    m_windowDim = size;
    m_windowShift = {};
    if(m_backend)
        recalc();
}

void Graphics::setWindowShift(FCoords shift) {
    m_windowShift = shift;
    m_coords[window] = m_coords[window0];
    m_coords[window].origin += m_windowShift;
}

void Graphics::setViewport(FCoords origin, FCoords size) {
    Log::debug<Log::graphics>("viewport origin ", origin, " size ", size);
    m_backend->setViewport(origin.round(), size.round()); // TODO check no-op
    recalc();
    m_backend->resizeBuffers();
    m_instance.screens().resize();
}

void Graphics::setScissor(ICoords from, ICoords to) {
    m_backend->setScissor(from, to);
}

void Graphics::releaseScissor() {
    m_backend->releaseScissor();
}

void Graphics::newFrame() {
    m_backend->newFrame();
}

void Graphics::present(TextureTarget& target) {
    m_backend->present(target);
}

void Graphics::recalc() {
    auto displayDim = FCoords{m_backend->display().size()};
    m_coords[null].size = displayDim;
    float scale0 = std::min(displayDim.x / baseDim.x, displayDim.y / baseDim.y);
    m_coords[base] = {(displayDim - scale0 * baseDim) / 2.f, scale0, baseDim};
    constexpr float buttonStripWidth = 64.f;
    float stripSize = buttonStripWidth * scale0;
    float scale1 = std::min((displayDim.x - stripSize) / m_windowDim.x, displayDim.y / m_windowDim.y);
    float scale2 = std::min(displayDim.x / m_windowDim.x, (displayDim.y - stripSize) / m_windowDim.y);
    float scale = std::max(scale1, scale2);
    bool vert = scale1 > scale2;
    FCoords center = vert ? FCoords{stripSize / 2.f, displayDim.y / 2.f} : FCoords{displayDim.x / 2.f, stripSize / 2.f};
    FCoords principal = vert ? FCoords{0.f, 1.f} : FCoords{1.f, 0.f};
    m_coords[buttons] = {center, scale0, {}, principal};
    FCoords reduceBase = vert ? FCoords{stripSize, 0.f} : FCoords{0.f, stripSize};
    FCoords reduceDim = displayDim - reduceBase;
    float scale3 = std::min(reduceDim.x / baseDim.x, reduceDim.y / baseDim.y);
    m_coords[reduced] = { reduceBase + (reduceDim - scale3 * baseDim) / 2.f, scale3, baseDim};
    m_coords[window0] = {(displayDim - reduceBase - scale * m_windowDim) / 2.f + reduceBase, scale, m_windowDim};
    m_coords[window] = m_coords[window0];
    m_coords[window].origin += m_windowShift;
}

ImageRef Graphics::addImage(std::unique_ptr<Image>&& ptr) {
    ImageRef ref{m_instance, ptr};
    m_images.insert(std::move(ptr));
    if(m_backend)
        ref->render(m_instance);
    return ref;
}

void Graphics::deleteImage(const ImageRef& ref) noexcept {
    auto it = std::find_if(m_images.begin(), m_images.end(),
            [ptr = *ref](const std::unique_ptr<Image>& entry) { return entry.get() == ptr; });
    if(it != m_images.end())
        m_images.erase(it);
}

std::array<TextureTarget, 2>& Graphics::blurTargets() const {
    return m_backend->m_blurTargets;
}

TextureTarget& Graphics::offscreenTarget() const {
    return m_backend->m_offscreenTarget;
}

Shaders& Graphics::shaders() const {
    return m_backend->m_shaders;
}
