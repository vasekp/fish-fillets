#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
    public:
        template<typename... NativeArgs>
        Display(const NativeArgs& ... nativeArgs);
        Display(Display&&) noexcept = default;
        ~Display();

        void setViewport(ICoords origin, ICoords size);
        USize size() const { return m_size; }

        std::weak_ptr<int> ref() const { return {m_ref}; }

        void bind() const;
        void swap() const;

    private:
        util::ResetOnMove<EGLDisplay, EGL_NO_DISPLAY> m_display;
        util::ResetOnMove<EGLSurface, EGL_NO_SURFACE> m_surface;
        util::ResetOnMove<EGLContext, EGL_NO_CONTEXT> m_context;
        USize m_size;
        ICoords m_origin;
        ICoords m_vpSize;
        std::shared_ptr<int> m_ref;

    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
