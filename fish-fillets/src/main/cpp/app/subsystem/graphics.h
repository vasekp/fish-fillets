#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "instance.h"

#include "graphics/platform-fwd.h"

#include "graphics/texturetype.h"
#include "graphics/imagedata.h"
#include "graphics/image.h"
#include "graphics/coords.h"
#include "graphics/shaders.h"
#include "graphics/ifont.h"
#include "graphics/drawtarget.h"
#include "graphics/texturetarget.h"

class GraphicsSystem;

class Graphics {
    Instance& m_instance;
    std::unique_ptr<GraphicsSystem> m_system;
    std::set<std::unique_ptr<Image>> m_images;
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
    Graphics(Instance& instance);
    ~Graphics();

    void activate(Platform::Display&& display);
    void shutdown();

    auto& system() const { return *m_system; }
    std::array<TextureTarget, 2>& blurTargets() const;
    TextureTarget& offscreenTarget() const;
    Shaders& shaders() const;
    bool ready() const { return (bool)m_system; }

    const Coords& coords(CoordSystems which) { return m_coords[which]; }

    void setWindowSize(FCoords size);
    void setWindowShift(FCoords shift);
    void setViewport(FCoords origin, FCoords size);
    void setScissor(ICoords from, ICoords to);
    void releaseScissor();
    void newFrame();
    void present(TextureTarget& target);

    ImageRef addImage(std::unique_ptr<Image>&& ptr);
    void deleteImage(const ImageRef& ref) noexcept;

    static constexpr FCoords baseDim{640, 480};

private:
    void recalc();

    static constexpr float buttonStripWidth = 64.f;
};

namespace decoders {
    ImageData png(Instance& instance, const std::string& filename);
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_GRAPHICS_H
