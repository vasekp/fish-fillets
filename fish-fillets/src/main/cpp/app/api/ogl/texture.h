#ifndef FISH_FILLETS_OGL_TEXTURE_H
#define FISH_FILLETS_OGL_TEXTURE_H

#include "subsystem/graphics/texturetype.h"

namespace ogl {

    class Texture {
        std::weak_ptr<int> m_ref;
        GLuint m_name = 0;
        USize m_size;
        unsigned m_channels;

    public:
        Texture(const ogl::Display& display, USize size, TextureType type, std::uint8_t* data);

        Texture() = default;
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;
        ~Texture();

        operator GLuint() const { return m_name; }
        auto size() const { return m_size; }

        void bind() const;
        void replaceData(std::uint8_t* data);
    };

}

#endif //FISH_FILLETS_OGL_TEXTURE_H
