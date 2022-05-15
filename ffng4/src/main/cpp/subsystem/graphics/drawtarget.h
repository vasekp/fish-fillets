#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
public:
    virtual void bind() const = 0;
    virtual FCoords size() const = 0;
    virtual ~DrawTarget() { }

    void blit(TextureView source, const ogl::Program &program,
              float destX = 0, float destY = 0, float srcX = 0, float srcY = 0,
              unsigned width = fullSize, unsigned height = fullSize) const;

    void blitFlip(TextureView source, const ogl::Program &program, bool flipX, bool flipY,
                  float destX = 0, float destY = 0, float srcX = 0, float srcY = 0,
                  unsigned width = fullSize, unsigned height = fullSize) const;

    static constexpr unsigned fullSize = std::numeric_limits<decltype(fullSize)>::max();
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H