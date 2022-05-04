#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
    std::shared_ptr<ogl::Display> m_display;
    Canvas m_canvas;
    ReadBuffer m_readBuffer;
    Shaders m_shaders;

public:
    GraphicsSystem(Instance& instance) :
        m_display(std::make_shared<ogl::Display>(instance.app()->window)),
        m_canvas(m_display->width(), m_display->height()),
        m_readBuffer(m_display),
        m_shaders(m_display, instance)
    { }

    auto& display() { return *m_display; }
    auto& ref() { return m_display; }

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
