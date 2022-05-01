#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
    ogl::Display m_display;
    Canvas m_canvas;
    Shaders m_shaders;

public:
    GraphicsSystem(Instance* instance) :
            m_display(instance->app->window),
            m_canvas(1000, 1000),
            m_shaders(instance)
    { }

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
