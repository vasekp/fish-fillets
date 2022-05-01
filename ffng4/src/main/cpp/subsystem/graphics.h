#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics/shaders.h"
#include "graphics/canvas.h"
#include "graphics/image.h"
#include "graphics/system.h"

class Graphics {
    Instance *instance;
    std::unique_ptr<GraphicsSystem> system;

public:
    Graphics(Instance* instance_) : instance(instance_) { }

    void activate();
    void shutdown();

    const auto* display() const { return &system->display; }
    const auto* canvas() const { return &system->canvas; }
    const auto* shaders() const { return &system->shaders; }

    void setCanvasSize(unsigned width, unsigned height);

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
