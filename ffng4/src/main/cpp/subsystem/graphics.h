#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"
#include "api/ogl.h"

#include "graphics/shaders.h"
#include "graphics/image.h"
#include "graphics/ifont.h"
#include "graphics/readbuffer.h"
#include "graphics/textureview.h"
#include "graphics/coords.h"
#include "graphics/displaytarget.h"
#include "graphics/texturetarget.h"
#include "graphics/system.h"
#include "graphics/graphicsutils.h"

class Graphics {
    Instance& m_instance;
    std::unique_ptr<GraphicsSystem> m_system;
    std::vector<Image*> m_images;
    FCoords m_windowDim;

public:
    enum CoordSystems {
        null,
        base,
        buttons,
        reduced,
        window,
        SIZE
    };

private:
    std::array<Coords, SIZE> m_coords;

public:
    Graphics(Instance& instance) : m_instance(instance) { }

    void activate();
    void shutdown();

    auto& system() const { return *m_system; }
    auto& display() const { return *m_system->m_display; }
    auto& fullscreenTarget() const { return m_system->m_fullscreenTarget; }
    auto& blurTargets() const { return m_system->m_blurTargets; }
    auto& offscreenTarget() const { return m_system->m_offscreenTarget; }
    auto& readBuffer() const { return m_system->m_readBuffer; }
    auto& shaders() const { return m_system->m_shaders; }
    bool ready() const { return (bool)m_system; }

    const Coords& coords(CoordSystems which) { return m_coords[which]; }

    void setWindowSize(unsigned width, unsigned height);
    void notifyDisplayResize();
    void setMask(const Image* image);
    void setMask(const ogl::Texture& texture);

    void regImage(Image*);
    void regImageMove(Image*, Image*) noexcept;
    void unregImage(Image*) noexcept;

    static constexpr FCoords baseDim{640, 480};

private:
    void recalc();
};

namespace decoders {
    ogl::Texture png(Instance& instance, const std::string& filename);
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_GRAPHICS_H