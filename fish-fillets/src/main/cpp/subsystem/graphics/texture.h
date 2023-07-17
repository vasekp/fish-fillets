#ifndef FISH_FILLETS_PIXMAP_H
#define FISH_FILLETS_PIXMAP_H

class GraphicsSystem;

class Texture {
    ogl::Texture m_native;
    ICoords m_physSize;
    FCoords m_logSize;

public:
    Texture() = default;
    Texture(Texture&& other) = default;
    Texture& operator=(Texture&& other) = default;

    Texture(ogl::Texture&& texture);
    Texture(GraphicsSystem& system, FCoords logSize, ICoords physSize);
    Texture(GraphicsSystem& system, void *data, ICoords size, int channels = 4);

    FCoords logSize() const { return m_logSize; }
    ICoords physSize() const { return m_physSize; }
    const ogl::Texture& native() const { return m_native; }
    void bind() const;
};

#endif //FISH_FILLETS_PIXMAP_H



