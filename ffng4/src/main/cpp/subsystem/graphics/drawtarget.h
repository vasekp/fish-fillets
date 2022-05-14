#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
    FCoords m_size;

public:
    DrawTarget(unsigned width = 0, unsigned height = 0) : m_size(width, height) { }

    void setSize(unsigned width, unsigned height);
    virtual void bind() const = 0;

    void blit(const ogl::Texture& texture, const ogl::Program &program,
              float destX = 0, float destY = 0, float srcX = 0, float srcY = 0,
              unsigned width = fullSize, unsigned height = fullSize,
              unsigned dstWidth = fullSize, unsigned dstHeight = fullSize) const;

    void blitFlip(const ogl::Texture& texture, const ogl::Program &program, bool flipX, bool flipY,
                  float destX = 0, float destY = 0, float srcX = 0, float srcY = 0,
                  unsigned width = fullSize, unsigned height = fullSize,
                  unsigned dstWidth = fullSize, unsigned dstHeight = fullSize) const;

    static constexpr unsigned fullSize = std::numeric_limits<decltype(fullSize)>::max();

protected:
    virtual void resize(unsigned width, unsigned height) = 0;
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H