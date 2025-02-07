#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

#include "subsystem/graphics.h"

#ifdef FISH_FILLETS_USE_VULKAN
#include "api/vulkan.h"
#else
#include "api/ogl.h"
#endif

class GraphicsBackend {
public:
    GraphicsBackend(Instance& instance, BACKEND::Display&& display);
    ~GraphicsBackend();

    auto& display() { return m_display; }

    void newFrame();
    void bind(DrawTarget* target);
    void setViewport(ICoords origin, ICoords size);
    void setScissor(ICoords from, ICoords to);
    void releaseScissor();
    void present(TextureTarget& target);
    void notifyResize(USize sz);

private:
    Graphics& m_graphics;
    BACKEND::Display m_display;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    Shaders m_shaders;
    DrawTarget* m_curTarget;

    struct Detail;
    Detail* m_detail;
    Detail* backendDetail();

    USize blurTargetDims();

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
