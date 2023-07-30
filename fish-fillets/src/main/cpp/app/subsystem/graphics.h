#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"

#ifdef FISH_FILLETS_USE_VULKAN
#include "api/vulkan.h"
#else
#include "api/ogl.h"
#endif

#include "graphics/texturetype.h"
#include "graphics/texture.h"
#include "graphics/image.h"
#include "graphics/coords.h"
#include "graphics/shaders.h"
#include "graphics/ifont.h"
#include "graphics/drawtarget.h"
#include "graphics/displaytarget.h"
#include "graphics/texturetarget.h"
#include "graphics/graphicssystem.h"

class Graphics {
    Instance& m_instance;
    std::unique_ptr<GraphicsSystem> m_system;
    std::vector<Image*> m_images;
    FCoords m_windowDim;
    FCoords m_windowShift;

public:
    enum CoordSystems {
        null,
        base,
        buttons,
        reduced,
        window0,
        window,
        SIZE
    };

private:
    std::array<Coords, SIZE> m_coords;

public:
    Graphics(Instance& instance) : m_instance(instance) { }

    void activate(GraphicsSystem::PlatformDisplay&& display);
    void shutdown();

    auto& system() const { return *m_system; }
    auto& blurTargets() { return m_system->m_blurTargets; }
    auto& offscreenTarget() const { return m_system->m_offscreenTarget; }
    auto& shaders() const { return m_system->m_shaders; }
    bool ready() const { return (bool)m_system; }

    const Coords& coords(CoordSystems which) { return m_coords[which]; }

    void setWindowSize(FCoords size);
    void setWindowShift(FCoords shift);
    void setViewport(FCoords origin, FCoords size);

    void regImage(Image*);
    void regImageMove(Image*, Image*) noexcept;
    void unregImage(Image*) noexcept;

    static constexpr FCoords baseDim{640, 480};

private:
    void recalc();
};

namespace decoders {
    Texture png(Instance& instance, const std::string& filename, TextureType type);
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_GRAPHICS_H
