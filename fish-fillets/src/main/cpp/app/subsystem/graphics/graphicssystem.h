#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
private:
    Graphics& m_graphics;
    std::shared_ptr<ogl::Display> m_display;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    Shaders m_shaders;
    DisplayTarget* m_curTarget;

public:
    template<typename... NativeArgs>
    GraphicsSystem(Instance& instance, const NativeArgs& ... nativeArgs) :
            m_graphics(instance.graphics()),
            m_display(std::make_shared<ogl::Display>(nativeArgs...)),
            m_blurTargets{*this, *this},
            m_offscreenTarget(*this),
            m_shaders(instance, *this)
    {
        resizeBuffers();
    }

    auto& ref() { return m_display; }

    ICoords displaySize() { return m_display->size(); }

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
