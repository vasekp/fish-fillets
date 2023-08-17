#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay m_display;
        EGLSurface m_surface;
        EGLContext m_context;
        USize m_size;
        ICoords m_origin;
        ICoords m_vpSize;
        std::shared_ptr<int> m_ref;

    public:
        template<typename... NativeArgs>
        Display(const NativeArgs& ... nativeArgs);
        Display(const Display&) = delete;
        Display(Display&&) noexcept;
        Display& operator=(const Display&) = delete;
        Display& operator=(Display&&) noexcept;
        ~Display();

        void setViewport(ICoords origin, ICoords size);
        USize size() const { return m_size; }

        std::weak_ptr<int> ref() const { return {m_ref}; }

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
