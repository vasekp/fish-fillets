#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics-shaders.h"
#include "graphics-canvas.h"

class Graphics {
public:
    std::unique_ptr<ogl::Display> display;
    std::unique_ptr<Canvas> canvas;
    std::unique_ptr<Shaders> shaders;
    std::unique_ptr<ogl::Texture> bg;

    Graphics(Instance*);
};

#endif //FISH_FILLETS_GRAPHICS_H
