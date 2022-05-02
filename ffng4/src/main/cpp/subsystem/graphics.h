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
    std::shared_ptr<GraphicsSystem> m_system; // Never really shared, but has weak_ptrs
    struct {
        int vx, vy, vw, vh;
    } viewport;

public:
    Graphics(Instance* instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    const auto system() const { return m_system; }
    const auto* display() const { return &m_system->m_display; }
    const auto* canvas() const { return &m_system->m_canvas; }
    const auto* readBuffer() const { return &m_system->m_readBuffer; }
    const auto* shaders() const { return &m_system->m_shaders; }

    void setCanvasSize(unsigned width, unsigned height);
    void drawFrame();
    std::pair<int, int> screen2canvas(int x, int y);
    std::pair<int, int> canvas2screen(int x, int y);

    ogl::Texture loadImage(const std::string& path) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
