#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
    Coords m_size;
    static constexpr unsigned fullSize = (unsigned)(-1);

public:
    void setSize(unsigned width, unsigned height);
    virtual void bind() const = 0;

    void blit(const ogl::Texture& texture, const ogl::Program &program, int destX = 0, int destY = 0,
              int srcX = 0, int srcY = 0, unsigned width = fullSize, unsigned height = fullSize) const;

protected:
    virtual void resize(unsigned width, unsigned height) = 0;
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H