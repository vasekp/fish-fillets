#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
public:
    virtual ~DrawTarget() { }

    virtual void bind() const = 0;
    virtual FCoords size() const = 0;

public:
    void draw(const ogl::Program &program, const Coords& coords, Program::Params params);
    void draw(TextureView image, const ogl::Program& program, const Coords& coords, Program::Params params = {});

    void draw(const Program &program, const Coords& coords, Program::Params params);
    void draw(TextureView image, const Program& program, const Coords& coords, Program::Params params = {});

protected:
    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
