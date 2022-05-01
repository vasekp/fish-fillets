#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        GLuint name = 0;
        Texture _texture;

    public:
        Framebuffer(GLuint width, GLuint height);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() const { return name; }
        auto width() const { return _texture.width(); }
        auto height() const { return _texture.height(); }
        auto& texture() const { return _texture; }

        void bind() const;
        void invalidate() { LOGV("framebuffer: detach %d", name); name = 0; }
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
