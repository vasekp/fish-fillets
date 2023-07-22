#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;
        ICoords m_origin;
        ICoords m_size;

    public:
        template<typename... NativeArgs>
        Display(const NativeArgs& ... nativeArgs);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        void setViewport(ICoords origin, ICoords size);
        ICoords size() const;

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
