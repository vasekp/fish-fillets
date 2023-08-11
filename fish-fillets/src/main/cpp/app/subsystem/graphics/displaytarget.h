#ifndef FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
#define FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H

#ifndef FISH_FILLETS_USE_VULKAN
class DisplayTarget : public DrawTarget {
    const ogl::Display& m_display;
    FCoords m_origin;
    FCoords m_size;

public:
    DisplayTarget(GraphicsSystem& system, const ogl::Display& display) :
        DrawTarget(system), m_display(display) { }

    FCoords size() const override { return FCoords{m_display.size()}; }

    void bind() override { m_display.bind(); }

private:
    bool flipY() const override { return true; }
};
#endif

#endif //FISH_FILLETS_GRAPHICS_DISPLAYTARGET_H
