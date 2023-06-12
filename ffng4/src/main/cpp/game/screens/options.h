#ifndef FISH_FILLETS_SCREENS_OPTIONS_H
#define FISH_FILLETS_SCREENS_OPTIONS_H

class OptionsOverlay : public GameScreen {
    BaseInput m_input;
    PNGImage m_options1;
    PNGImage m_options2;
    FCoords m_origin;
    bool m_visible;

public:
    OptionsOverlay(Instance& instance);

    void show();
    void hide();
    bool visible() const { return m_visible; }

    IInputSink & input() override { return m_input; }

    void own_draw(const DrawTarget &target, float dt) override;
    bool own_pointer(FCoords coords, bool longPress) override;

private:
    constexpr static FCoords imgSize{195, 332};
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H