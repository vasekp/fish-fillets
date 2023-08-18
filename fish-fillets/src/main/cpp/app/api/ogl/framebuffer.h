#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

// always included from ogl.h

namespace ogl {

    class Framebuffer {
    public:
        Framebuffer(const ogl::Display& display, const ogl::Texture& texture);
        Framebuffer(Framebuffer&&) = default;
        ~Framebuffer();

        operator GLuint() const { return m_name; }

        void bind() const;
        void unbind() const;
        void texture(const ogl::Texture& texture);

    private:
        std::weak_ptr<int> m_ref;
        const Texture* m_texture;
        util::ResetOnMove<GLuint, 0> m_name;
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
