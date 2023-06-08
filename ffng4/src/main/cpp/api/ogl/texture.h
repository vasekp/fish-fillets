#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

namespace ogl {

    class Texture {
        std::weak_ptr<const Display> m_ref;
        GLuint m_name = 0;
        GLuint m_width;
        GLuint m_height;

    public:
        static Texture fromImageData(const std::shared_ptr<ogl::Display>& ref, GLuint width, GLuint height, std::size_t stride, void *data, int channels = 4);
        static Texture empty(const std::shared_ptr<ogl::Display>& ref, GLuint width, GLuint height);

        Texture() = default;
        Texture(const Texture &) = delete;
        Texture& operator=(const Texture &) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        ~Texture();

        operator GLuint() const { return m_name; }
        auto width() const { return m_width; }
        auto height() const { return m_height; }
        bool live() const { return m_name && !m_ref.expired(); }

        void bind() const;

    private:
        Texture(const std::shared_ptr<ogl::Display>& ref, GLuint width, GLuint height);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H
