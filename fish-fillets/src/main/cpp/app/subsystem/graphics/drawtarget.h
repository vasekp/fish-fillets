#ifndef FISH_FILLETS_GRAPHICS_DRAWTARGET_H
#define FISH_FILLETS_GRAPHICS_DRAWTARGET_H

class GraphicsBackend;

class DrawTarget {
public:
    virtual ~DrawTarget() { }

    DrawTarget(GraphicsBackend& backend) : m_backend(backend) { }
    virtual FCoords size() const = 0;

    void draw(const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {});
    void draw(BaseProgram::Shape shape,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {});
    void draw(const Texture& image,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {});
    void draw(const Image* image,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {});
    void draw(BaseProgram::Textures textures,
        const BaseProgram& program, const Coords& coords, BaseProgram::Params params = {});

    virtual void bind() = 0;

protected:
    GraphicsBackend& m_backend;

    void drawMain(const BaseProgram& program, const Coords& coords, BaseProgram::Params params, BaseProgram::Shape shape, const BaseProgram::Textures& textures);

    virtual bool flipY() const { return false; }
};

#endif //FISH_FILLETS_GRAPHICS_DRAWTARGET_H
