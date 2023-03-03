#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;
        FCoords m_origin;
        FCoords m_size;

    public:
        Display(NativeWindowType window);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        void setViewport(FCoords origin, FCoords size);
        std::pair<FCoords, FCoords> getViewport() const;
        FCoords size() const;

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
