#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

#include "subsystem/graphics/texturetype.h"

namespace ogl {

    class Texture {
        std::weak_ptr<int> m_ref;
        GLuint m_name = 0;
        GLuint m_width;
        GLuint m_height;
        unsigned m_channels;

    public:
        static Texture fromImageData(const ogl::Display& display, GLuint width, GLuint height, TextureType type, void* data);
        static Texture empty(const ogl::Display& display, GLuint width, GLuint height);

        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        ~Texture();

        operator GLuint() const { return m_name; }
        auto width() const { return m_width; }
        auto height() const { return m_height; }

        void bind() const;
        void replaceData(void* data);

    private:
        Texture(const std::weak_ptr<int>& ref, GLuint width, GLuint height, TextureType type, void* data);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H
