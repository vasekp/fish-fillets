#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class DrawTarget {
public:
    virtual ~DrawTarget() { }

    virtual void bind() const = 0;
    virtual FCoords size() const = 0;

public:
    void draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape = BaseProgram::Shape::rect);
    void draw(TextureView image, const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {}, BaseProgram::Shape shape = BaseProgram::Shape::rect);

protected:
    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
