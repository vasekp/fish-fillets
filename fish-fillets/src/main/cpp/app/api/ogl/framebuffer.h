#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        std::weak_ptr<int> m_ref;
        GLuint m_name = 0;

    public:
        Framebuffer(const ogl::Display& display);
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() const { return m_name; }

        void bind() const;
        void unbind() const;
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
