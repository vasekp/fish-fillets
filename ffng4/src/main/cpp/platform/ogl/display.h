#ifndef FISH_FILLETS_OGL_DISPLAY_H
#define FISH_FILLETS_OGL_DISPLAY_H

namespace ogl {

    class Display {
        EGLDisplay display;
        EGLSurface surface;
        EGLContext context;

        std::int32_t _width;
        std::int32_t _height;

    public:

        Display(ANativeWindow *window);
        Display(const Display &) = delete;
        Display &operator=(const Display &) = delete;
        ~Display();

        auto width() const { return _width; }
        auto height() const { return _height; }

        void bind() const;
        void swap() const;
    };

}

#endif //FISH_FILLETS_OGL_DISPLAY_H
