#include "subsystem/graphics.h"
#include "../graphicsbackend.h"

#include "displaytarget.h"

GraphicsBackend::GraphicsBackend(Instance& instance, BACKEND::Display&& display) :
    m_graphics(instance.graphics()),
    m_display(std::move(display)),
    m_blurTargets{{
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}},
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}}}},
    m_offscreenTarget{*this, m_display.size()},
    m_shaders{instance, *this}
{ }

GraphicsBackend::~GraphicsBackend() = default;

void GraphicsBackend::resizeBuffers() {
    auto dispSize = m_display.size();
    m_offscreenTarget.resize(dispSize);
    auto blurSize = blurTargetDims();
    m_blurTargets[0].resize(blurSize, FCoords{dispSize});
    m_blurTargets[1].resize(blurSize, FCoords{dispSize});
}

USize GraphicsBackend::blurTargetDims() {
    auto dispSize = FCoords{m_display.size()};
    auto scale = std::max(Graphics::baseDim.x / dispSize.x, Graphics::baseDim.y / dispSize.y);
    auto size = (scale * dispSize).toSize();
    Log::debug<Log::graphics>("Blur dims: ", size);
    return size;
}

void GraphicsBackend::newFrame() {
    m_offscreenTarget.bind();
    glClear(GL_COLOR_BUFFER_BIT);
    m_curTarget = &m_offscreenTarget;
}

void GraphicsBackend::bind(DrawTarget* target) {
    if(target == m_curTarget)
        return;
    target->bind();
    m_curTarget = target;
}

void GraphicsBackend::setViewport(ICoords origin, ICoords size) {
    m_display.setViewport(origin, size);
}

void GraphicsBackend::setScissor(ICoords from, ICoords to) {
    ICoords size = to - from;
    glScissor(from.x, from.y, size.x, size.y);
    glEnable(GL_SCISSOR_TEST);
}

void GraphicsBackend::releaseScissor() {
    glDisable(GL_SCISSOR_TEST);
}

void GraphicsBackend::present(TextureTarget& target) {
    DisplayTarget{*this, m_display}.draw(m_offscreenTarget.texture(), m_shaders.copy(), m_graphics.coords(Graphics::CoordSystems::null));
    m_display.swap();
}
