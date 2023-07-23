#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;
        ICoords m_origin;
        ICoords m_size;
        std::shared_ptr<int> m_ref;

    public:
        template<typename... NativeArgs>
        Display(const NativeArgs& ... nativeArgs);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        void setViewport(ICoords origin, ICoords size);
        auto width() const { return m_size.x; }
        auto height() const { return m_size.y; }
        ICoords size() const { return m_size; }

        std::weak_ptr<int> ref() const { return {m_ref}; }

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
