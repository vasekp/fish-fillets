#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

namespace ogl {

    class Texture {
        GLuint _width;
        GLuint _height;
        GLuint name{};

    public:
        static Texture fromImageData(GLuint width, GLuint height, std::size_t stride, void *data);
        static Texture empty(GLuint width, GLuint height);

        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        ~Texture();

        operator GLuint() const { return name; }
        auto width() const { return _width; }
        auto height() const { return _height; }

        void bind();

    private:
        Texture(GLuint width, GLuint height);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H
