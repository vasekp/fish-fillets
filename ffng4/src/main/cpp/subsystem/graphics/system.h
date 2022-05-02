#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
    ogl::Display m_display;
    Canvas m_canvas;
    ogl::Framebuffer m_readBuffer;
    Shaders m_shaders;

public:
    GraphicsSystem(Instance* instance) :
            m_display(instance->app->window),
            m_canvas(),
            m_readBuffer(),
            m_shaders(instance)
    { }

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
