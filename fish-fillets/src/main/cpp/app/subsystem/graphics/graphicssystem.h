#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

struct PlatformDetail;

class GraphicsSystem {
public:
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformDisplay = vulkan::Display;
#else
    using PlatformDisplay = ogl::Display;
#endif

private:
    Graphics& m_graphics;
    PlatformDisplay m_display;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    Shaders m_shaders;
    DisplayTarget* m_curTarget;
    std::unique_ptr<PlatformDetail> m_platform;

public:
    GraphicsSystem(Instance& instance, PlatformDisplay&& display);
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
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
