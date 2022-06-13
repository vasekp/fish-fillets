#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;

    public:
        Display(NativeWindowType window);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        std::int32_t width() const;
        std::int32_t height() const;

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
