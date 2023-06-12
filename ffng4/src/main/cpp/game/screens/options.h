#ifndef FISH_FILLETS_SCREENS_OPTIONS_H
#define FISH_FILLETS_SCREENS_OPTIONS_H

class OptionsOverlay : public GameScreen {
    BaseInput m_input;
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
        Subtitles type;
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
    } m_volbars[3];

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
    constexpr static float volLength = 120;
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H