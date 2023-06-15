#ifndef FISH_FILLETS_SCREENS_OPTIONS_H
#define FISH_FILLETS_SCREENS_OPTIONS_H

class OptionsOverlay : public GameScreen, public IInputSink {
    PNGImage m_options;
    PNGImage m_slider;
    FCoords m_origin;
    bool m_visible;

    struct Button {
        std::string value;
        PNGImage image;
        FCoords origin;
        static constexpr FCoords size{47, 33};
    } m_buttons[3];

    struct VolumeBar {
        AudioType type;
        const char* typeString;
        FCoords origin;
        FCoords from;
        FCoords to;

        VolumeBar(AudioType type_, const char* typeString_, FCoords origin_);
    };

    const VolumeBar m_volbars[3];
    const VolumeBar* m_sliding;

    std::string m_currSubs;

public:
    OptionsOverlay(Instance& instance);

    void show();
    void hide();
    bool visible() const { return m_visible; }

    IInputSink& input() override { return *this; }

    void own_draw(const DrawTarget &target, float dt) override;
    bool pointerDown(FCoords coords) override;
    bool pointerMove(FCoords coords) override;
    bool pointerUp(bool empty) override;
    void pointerCancel() override;
    bool keyDown(Key key) override;

private:
    static float exp(float x);
    static float log(float v);

    constexpr static FCoords imgSize{195, 332};
    constexpr static float volLength = 120.f;
    constexpr static float volSliderOffset = 8.f;
    constexpr static float volTolerance = 10.f;
    constexpr static float volSpan = 63.f;
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H