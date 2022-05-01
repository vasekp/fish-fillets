#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;

        std::int32_t m_width;
        std::int32_t m_height;

    public:

        Display(ANativeWindow *window);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        auto width() const { return m_width; }
        auto height() const { return m_height; }

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
