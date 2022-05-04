#ifndef FISH_FILLETS_GRAPHICS_READBUFFER_H
#define FISH_FILLETS_GRAPHICS_READBUFFER_H

class ReadBuffer {
    ogl::Framebuffer m_buffer;

public:
    ReadBuffer() : m_buffer() { }

    void setImage(const Image& image) const {
        m_buffer.bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, image.texture(), 0);
        m_buffer.unbind();
    }

    Color getPixel(unsigned x, unsigned y) const {
        std::uint8_t pixels[4];
        m_buffer.bind();
        glReadPixels((GLint)x, (GLint)y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        m_buffer.unbind();
        return {pixels[0], pixels[1], pixels[2]};
    }
};

#endif //FISH_FILLETS_GRAPHICS_READBUFFER_H
