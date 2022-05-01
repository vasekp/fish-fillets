#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        GLuint m_name = 0;
        Texture m_texture;

    public:
        Framebuffer(GLuint width, GLuint height);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() const { return m_name; }
        auto width() const { return m_texture.width(); }
        auto height() const { return m_texture.height(); }
        auto& texture() const { return m_texture; }

        void bind(GLuint texture = 0) const;
        void invalidate() { LOGV("framebuffer: detach %d", m_name); m_name = 0; }
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
