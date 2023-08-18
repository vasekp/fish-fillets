#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

class DisplayTarget : public DrawTarget {
public:
    DisplayTarget(GraphicsBackend& backend, const ogl::Display& display) :
        DrawTarget(backend), m_display(display) { }

    FCoords size() const override { return FCoords{m_display.size()}; }

    void bind() override { m_display.bind(); }

private:
    const ogl::Display& m_display;
    FCoords m_origin;
    FCoords m_size;

    bool flipY() const override { return true; }
};

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
