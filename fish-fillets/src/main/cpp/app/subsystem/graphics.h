#ifndef FISH_FILLETS_GRAPHICS_H
#define FISH_FILLETS_GRAPHICS_H

#include "common.h"
#include "instance.h"
#include "graphics/backend-fwd.h"
#include "graphics/texturetype.h"
#include "graphics/texture.h"
#include "graphics/imagedata.h"
#include "graphics/image.h"
#include "graphics/coords.h"
#include "graphics/shaders.h"
#include "graphics/ifont.h"
#include "graphics/drawtarget.h"
#include "graphics/texturetarget.h"

class Graphics {
public:
    Graphics(Instance& instance);
    ~Graphics();

    void activate(BACKEND::Display&& display);
    void shutdown();

    auto& backend() const { return *m_backend; }
    std::array<TextureTarget, 2>& blurTargets() const;
    TextureTarget& offscreenTarget() const;
    Shaders& shaders() const;
    bool ready() const { return (bool)m_backend; }

    enum CoordSystems {
        null,
        base,
        buttons,
        reduced,
        window0,
        window,
        SIZE
    };

    const Coords& coords(CoordSystems which) { return m_coords[which]; }

    void setWindowSize(FCoords size);
    void setWindowShift(FCoords shift);
    void notifyResize(USize sz);
    void setScissor(ICoords from, ICoords to);
    void releaseScissor();
    void newFrame();
    void present(TextureTarget& target);

    ImageRef addImage(std::unique_ptr<Image>&& ptr);
    void deleteImage(const ImageRef& ref) noexcept;

    static constexpr FCoords baseDim{640, 480};

private:
    Instance& m_instance;
    std::unique_ptr<GraphicsBackend> m_backend;
    std::set<std::unique_ptr<Image>> m_images;
    FCoords m_windowDim;
    FCoords m_windowShift;
    std::array<Coords, SIZE> m_coords;

    void recalc();
};

namespace decoders {
    ImageData png(Instance& instance, const std::string& filename);
    std::unique_ptr<IFont> ttf(Instance& instance, const std::string& filename);
}

#endif //FISH_FILLETS_GRAPHICS_H
