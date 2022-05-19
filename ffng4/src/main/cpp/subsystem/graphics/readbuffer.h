#ifndef FISH_FILLETS_GRAPHICS_READBUFFER_H
#define FISH_FILLETS_GRAPHICS_READBUFFER_H

class ReadBuffer {
    ogl::Framebuffer m_buffer;

public:
    ReadBuffer(const std::shared_ptr<ogl::Display>& ref);

    void setImage(const Image* image) const;
    Color getPixel(unsigned x, unsigned y) const;
};

#endif //FISH_FILLETS_GRAPHICS_READBUFFER_H