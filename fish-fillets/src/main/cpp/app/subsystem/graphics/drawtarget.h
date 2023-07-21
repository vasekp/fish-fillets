#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class GraphicsSystem;

class DrawTarget {
protected:
    GraphicsSystem& m_system;

public:
    virtual ~DrawTarget() { }

    DrawTarget(GraphicsSystem& system) : m_system(system) { }
    virtual FCoords size() const = 0;

    void clear();

    void draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape = BaseProgram::Shape::rect);
    void draw(const Texture& image, const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {}, BaseProgram::Shape shape = BaseProgram::Shape::rect);
    void draw(const Image* image, const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {}, BaseProgram::Shape shape = BaseProgram::Shape::rect);

    virtual void bind() = 0;

protected:
    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
