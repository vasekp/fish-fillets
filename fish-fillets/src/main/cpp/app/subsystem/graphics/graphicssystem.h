#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

#include "platform-full.h"
#include "displaytarget.h"
#include "texturetarget.h"

struct PlatformDetail;

class GraphicsSystem {
private:
    Graphics& m_graphics;
    Platform::Display m_display;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    Shaders m_shaders;
    DrawTarget* m_curTarget;
    std::unique_ptr<PlatformDetail> m_platform;

public:
    GraphicsSystem(Instance& instance, Platform::Display&& display);
    ~GraphicsSystem();

    auto& display() { return m_display; }

    void newFrame();
    void bind(DrawTarget* target);
    void setViewport(ICoords origin, ICoords size);
    void setScissor(ICoords from, ICoords to);
    void releaseScissor();
    void present(TextureTarget& target);

    void resizeBuffers();

    friend class Graphics;

private:
    std::unique_ptr<PlatformDetail> platformDetail();

    USize blurTargetDims();
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
