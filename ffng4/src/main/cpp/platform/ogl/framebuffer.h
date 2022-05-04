#ifndef FISH_FILLETS_OGL_FRAMEBUFFER_H
#define FISH_FILLETS_OGL_FRAMEBUFFER_H

namespace ogl {

    class Framebuffer {
        GLuint m_name = 0;

    public:
        Framebuffer();
        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;
        ~Framebuffer();

        operator GLuint() const { return m_name; }

        void bind() const;
        static void unbind() ;
        void invalidate() { LOGV("framebuffer: detach %d", m_name); m_name = 0; }
    };

}

#endif //FISH_FILLETS_OGL_FRAMEBUFFER_H
