#ifndef FISH_FILLETS_SCREENS_OPTIONS_H
#define FISH_FILLETS_SCREENS_OPTIONS_H

class OptionsOverlay : public GameScreen, public IInputSink {
    PNGImage m_options;
    PNGImage m_slider;
    FCoords m_origin;
    bool m_visible;

    enum class Subtitles {
        cz,
        en,
        none
    };

    struct Button {
        Subtitles value;
        PNGImage image;
        FCoords origin;
        static constexpr FCoords size{47, 33};
    } m_buttons[3];

    enum class Volumes {
        sound,
        music,
        talk
    };

    struct VolumeBar {
        Volumes type;
        FCoords origin;
        float value;
        FCoords from;
        FCoords to;

        VolumeBar(Volumes type_, FCoords origin_);
    };

    VolumeBar m_volbars[3];
    VolumeBar* m_sliding;

    Subtitles m_subs;

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
    constexpr static FCoords imgSize{195, 332};
    constexpr static float volLength = 120.f;
    constexpr static float volSliderOffset = 8.f;
    constexpr static float volTolerance = 10.f;
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H