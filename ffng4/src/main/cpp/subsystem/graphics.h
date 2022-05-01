#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics/shaders.h"
#include "graphics/canvas.h"
#include "graphics/image.h"

class Graphics {
    Instance *instance;

public:
    std::unique_ptr<ogl::Display> display;
    std::unique_ptr<Canvas> canvas;
    std::unique_ptr<Shaders> shaders;

    Graphics(Instance*);

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
