#include "subsystem/graphics/graphicsbackend.h"

GraphicsBackend::GraphicsBackend(Instance& instance, BACKEND::Display&& display) :
    m_graphics(instance.graphics()),
    m_display(std::move(display)),
    m_blurTargets{{
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}},
        TextureTarget{*this, blurTargetDims(), FCoords{m_display.size()}}}},
    m_offscreenTarget{*this, m_display.size()},
    m_shaders{instance, *this},
    m_detail{backendDetail()}
{ }

USize GraphicsBackend::blurTargetDims() {
    auto dispSize = FCoords{m_display.size()};
    auto scale = std::max(Graphics::baseDim.x / dispSize.x, Graphics::baseDim.y / dispSize.y);
    auto size = (scale * dispSize).toSize();
    Log::debug<Log::graphics>("Blur dims: ", size);
    return size;
}

void GraphicsBackend::notifyResize(USize sz) {
    m_display.notifyResize(sz);
    m_offscreenTarget.resize(sz);
    auto blurSize = blurTargetDims();
    m_blurTargets[0].resize(blurSize, FCoords{sz});
    m_blurTargets[1].resize(blurSize, FCoords{sz});
}
