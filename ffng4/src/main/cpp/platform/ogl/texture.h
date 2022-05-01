#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

namespace ogl {

    class Texture {
        GLuint m_width;
        GLuint m_height;
        GLuint m_name = 0;

    public:
        static Texture fromImageData(GLuint width, GLuint height, std::size_t stride, void *data);
        static Texture empty(GLuint width, GLuint height);

        Texture() = default;
        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        ~Texture();

        operator GLuint() const { return m_name; }
        auto width() const { return m_width; }
        auto height() const { return m_height; }

        void bind() const;
        void invalidate() { if(m_name) LOGV("texture: detach %d", m_name); m_name = 0; }

    private:
        Texture(GLuint width, GLuint height);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H