#ifndef FISH_FILLETS_GRAPHICS_SYSTEM_H
#define FISH_FILLETS_GRAPHICS_SYSTEM_H

class GraphicsSystem {
private:
    std::shared_ptr<ogl::Display> m_display;
    DisplayTarget m_windowTarget;
    DisplayTarget m_fullscreenTarget;
    std::array<TextureTarget, 2> m_blurTargets;
    TextureTarget m_offscreenTarget;
    ReadBuffer m_readBuffer;
    Shaders m_shaders;

public:
    GraphicsSystem(Instance& instance);

    auto& display() { return *m_display; }
    auto& ref() { return m_display; }

    void setWindowSize(unsigned width, unsigned height);

    friend class Graphics;
};

#endif //FISH_FILLETS_GRAPHICS_SYSTEM_H