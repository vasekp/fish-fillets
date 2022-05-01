#ifndef FISH_FILLETS_GRAPHICS_CANVAS_H
#define FISH_FILLETS_GRAPHICS_CANVAS_H

class Canvas {
    ogl::Framebuffer m_framebuffer;
    unsigned m_width;
    unsigned m_height;

public:
    Canvas(unsigned maxWidth, unsigned maxHeight) :
            m_framebuffer(maxWidth, maxHeight),
            m_width(0),
            m_height(0)
    {
        glActiveTexture(Shaders::texCanvas_gl);
        glBindTexture(GL_TEXTURE_2D, m_framebuffer.texture());
        glActiveTexture(Shaders::texImage_gl);
    }

    auto width() const { return m_width; }
    auto height() const { return m_height; }
    auto texWidth() const { return m_framebuffer.width(); }
    auto texHeight() const { return m_framebuffer.height(); }

    void resize(unsigned width, unsigned height) {
        assert(width <= m_framebuffer.width() && height <= m_framebuffer.height());
        m_width = width;
        m_height = height;
    }

    void bind() const {
        m_framebuffer.bind();
        glViewport(0, 0, (GLint)width(), (GLint)height());
    }
};

#endif //FISH_FILLETS_GRAPHICS_CANVAS_H
