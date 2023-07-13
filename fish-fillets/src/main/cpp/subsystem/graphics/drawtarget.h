#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
public:
    virtual ~DrawTarget() { }

    virtual void bind() const = 0;
    virtual FCoords size() const = 0;

    struct Params {
        FCoords src{};
        FCoords dest{};
        FCoords srcSize{};
        FCoords area{};

        constexpr static Params defaults() { return {}; }
    };

public:
    void draw(const ogl::Program &program, const Coords& coords, Params params);
    void draw(TextureView image, const ogl::Program &program, const Coords& coords, Params params = Params::defaults());

protected:
    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
