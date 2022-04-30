#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        GLuint name{};
        Texture _texture;

    public:
        Framebuffer(GLuint width, GLuint height);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() { return name; }
        auto width() { return _texture.width(); }
        auto height() { return _texture.height(); }
        auto& texture() { return _texture; }

        void bind();
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
