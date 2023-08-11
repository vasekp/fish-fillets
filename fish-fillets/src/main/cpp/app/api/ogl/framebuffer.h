#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        std::weak_ptr<int> m_ref;
        const Texture* m_texture;
        GLuint m_name = 0;

    public:
        Framebuffer(const ogl::Display& display, const ogl::Texture& texture);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        Framebuffer(Framebuffer&&);
        Framebuffer& operator=(Framebuffer&&);
        ~Framebuffer();

        operator GLuint() const { return m_name; }

        void bind() const;
        void unbind() const;
        void texture(const ogl::Texture& texture);
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
