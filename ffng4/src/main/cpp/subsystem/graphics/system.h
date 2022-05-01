#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
    ogl::Display display;
    Canvas canvas;
    Shaders shaders;

public:
    GraphicsSystem(Instance* instance) :
        display(instance->app->window),
        canvas(1000, 1000),
        shaders(instance)
    { }

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H
