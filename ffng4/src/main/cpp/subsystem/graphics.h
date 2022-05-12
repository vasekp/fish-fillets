#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "platform/ogl.h"
#include "subsystem/files.h"

#include "graphics/shaders.h"
#include "graphics/image.h"
#include "graphics/readbuffer.h"
#include "graphics/displaytarget.h"
#include "graphics/offscreentarget.h"
#include "graphics/system.h"
#include "graphics/utils.h"

class Graphics {
    Instance& m_instance;
    std::unique_ptr<GraphicsSystem> m_system;
    unsigned m_windowWidth;
    unsigned m_windowHeight;

public:
    Graphics(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    auto& system() const { return *m_system; }
    auto& display() const { return m_system->m_display; }
    auto& displayTarget() const { return m_system->m_displayTarget; }
    auto& readBuffer() const { return m_system->m_readBuffer; }
    auto& shaders() const { return m_system->m_shaders; }

    void setWindowSize(unsigned width, unsigned height);
    void setMask(const Image &image);

    void drawFrame();

    ogl::Texture loadImage(const std::string& path) const;
    std::vector<ogl::Texture> renderText(const std::string& text) const;
    ogl::Texture renderLine(const std::string& text) const;
};

#endif //FISH_FILLETS_GRAPHICS_H