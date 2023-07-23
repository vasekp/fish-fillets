#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

namespace ogl {

    class Texture {
        std::weak_ptr<int> m_ref;
        GLuint m_name = 0;
        GLuint m_width;
        GLuint m_height;

    public:
        static Texture fromImageData(const ogl::Display& display, GLuint width, GLuint height, int channels, void *data);
        static Texture empty(const ogl::Display& display, GLuint width, GLuint height);

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
        Texture(const std::weak_ptr<int>& ref, GLuint width, GLuint height);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H
