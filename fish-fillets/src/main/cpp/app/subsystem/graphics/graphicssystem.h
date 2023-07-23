#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformDisplay = vulkan::Display;
#else
    using PlatformDisplay = ogl::Display;
#endif

    Graphics& m_graphics;
    PlatformDisplay m_display;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    Shaders m_shaders;
    DisplayTarget* m_curTarget;

#ifdef FISH_FILLETS_USE_VULKAN // TODO m_
    vk::raii::Semaphore imageAvailableSemaphore;
    vk::raii::Semaphore renderFinishedSemaphore;
    vk::raii::Fence inFlightFence;
    std::uint32_t m_curImageIndex;
    const vk::Image* m_curImage;
#endif

public:
    template<typename... NativeArgs>
    GraphicsSystem(Instance& instance, const NativeArgs& ... nativeArgs) :
            m_graphics(instance.graphics()),
            m_display{nativeArgs...},
            m_blurTargets{*this, *this},
            m_offscreenTarget(*this),
            m_shaders(instance, *this)
#ifdef FISH_FILLETS_USE_VULKAN
          , imageAvailableSemaphore{m_display.device(), vk::SemaphoreCreateInfo{}},
            renderFinishedSemaphore{m_display.device(), vk::SemaphoreCreateInfo{}},
            inFlightFence{m_display.device(), vk::FenceCreateInfo{}.setFlags(vk::FenceCreateFlagBits::eSignaled)}
#endif
    {
        resizeBuffers();
    }

    auto& display() { return m_display; }

    void newFrame();
    void bind(DrawTarget* target);
    void setViewport(ICoords origin, ICoords size);
    void setScissor(ICoords from, ICoords to);
    void releaseScissor();
    void present(TextureTarget& target);

    void resizeBuffers();

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
