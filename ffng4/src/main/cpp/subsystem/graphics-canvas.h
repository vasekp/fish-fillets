#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Canvas {
    ogl::Framebuffer framebuffer;
    unsigned _width;
    unsigned _height;

public:
    Canvas(unsigned maxWidth, unsigned maxHeight) :
            framebuffer(maxWidth, maxHeight),
            _width(0),
            _height(0)
    {
        glActiveTexture(Shaders::texCanvas_gl);
        glBindTexture(GL_TEXTURE_2D, framebuffer.texture());
        glActiveTexture(Shaders::texImage_gl);
    }

    auto width() { return _width; }
    auto height() { return _height; }
    auto texWidth() { return framebuffer.width(); }
    auto texHeight() { return framebuffer.height(); }

    void resize(unsigned width, unsigned height) {
        assert(width <= framebuffer.width() && height <= framebuffer.height());
        _width = width;
        _height = height;
    }

    void bind() {
        framebuffer.bind();
        glViewport(0, 0, (GLint)width(), (GLint)height());
    }
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
