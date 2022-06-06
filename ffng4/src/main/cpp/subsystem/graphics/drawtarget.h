#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
public:
    virtual ~DrawTarget() { }

    virtual void bind() const = 0;
    virtual FCoords size() const = 0;

    void blit(TextureView source, const ogl::Program &program,
              float destX = 0, float destY = 0, float srcX = 0, float srcY = 0,
              unsigned width = fullSize, unsigned height = fullSize) const;

    void fill(const ogl::Program &program, float x1, float y1, float x2, float y2) const;

    static constexpr unsigned fullSize = std::numeric_limits<unsigned>::max();

protected:
    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
