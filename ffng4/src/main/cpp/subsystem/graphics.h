#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "api/ogl.h"

#include "subsystem/files.h" // TODO
#include "graphics/shaders.h"
#include "graphics/image.h"
#include "graphics/readbuffer.h"
#include "graphics/textureview.h"
#include "graphics/displaytarget.h"
#include "graphics/texturetarget.h"
#include "graphics/system.h"
#include "graphics/graphicsutils.h"

class Graphics {
    Instance& m_instance;
    std::unique_ptr<GraphicsSystem> m_system;

public:
    Graphics(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    auto& system() const { return *m_system; }
    auto& display() const { return *m_system->m_display; }
    auto& windowTarget() const { return m_system->m_windowTarget; }
    auto& fullscreenTarget() const { return m_system->m_fullscreenTarget; }
    auto& blurTargets() const { return m_system->m_blurTargets; }
    auto& offscreenTarget() const { return m_system->m_offscreenTarget; }
    auto& readBuffer() const { return m_system->m_readBuffer; }
    auto& shaders() const { return m_system->m_shaders; }
    bool ready() const { return (bool)m_system; }
    float dpi() const;

    void setWindowSize(unsigned width, unsigned height, FCoords reserve = {});
    void setMask(const Image* image);
    void setMask(const ogl::Texture& texture);

    ogl::Texture loadPNG(const SystemFile& filename) const;
    ogl::Texture renderText(const std::string& text, const std::string& font /* TODO filesystem */, float fontSize, float outline) const;
};

#endif //FISH_FILLETS_GRAPHICS_H
