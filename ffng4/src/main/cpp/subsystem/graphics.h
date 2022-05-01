#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics/canvas.h"
#include "graphics/shaders.h"
#include "graphics/system.h"
#include "graphics/image.h"
#include "graphics/utils.h"

class Graphics {
    Instance *m_instance;
    std::shared_ptr<GraphicsSystem> m_system;

public:
    Graphics(Instance* instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    const auto system() const { return m_system; }
    const auto* display() const { return &m_system->m_display; }
    const auto* canvas() const { return &m_system->m_canvas; }
    const auto* shaders() const { return &m_system->m_shaders; }

    void setCanvasSize(unsigned width, unsigned height);

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
