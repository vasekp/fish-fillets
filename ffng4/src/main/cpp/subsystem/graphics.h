#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics/coords.h"
#include "graphics/canvas.h"
#include "graphics/shaders.h"
#include "graphics/image.h"
#include "graphics/readbuffer.h"
#include "graphics/system.h"
#include "graphics/utils.h"

class Graphics {
    Instance& m_instance;
    std::shared_ptr<GraphicsSystem> m_system; // Never really shared, but has weak_ptrs

public:
    Graphics(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    auto& system() const { return m_system; }
    auto& display() const { return m_system->m_display; }
    auto& canvas() const { return m_system->m_canvas; }
    auto& readBuffer() const { return m_system->m_readBuffer; }
    auto& shaders() const { return m_system->m_shaders; }

    void setMask(const Image &image);

    void drawFrame();

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
