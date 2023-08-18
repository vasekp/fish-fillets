#ifndef FISH_FILLETS_SCREENS_OPTIONS_H
#define FISH_FILLETS_SCREENS_OPTIONS_H

#include "common.h"
#include "instance.h"
#include "subsystem/audio/audiotype.h"
#include "subsystem/graphics/image.h"
#include "subsystem/input.h"

class DrawTarget;

class OptionsOverlay : public IInputSink {
public:
    OptionsOverlay(Instance& instance);

    void show();
    void hide();
    bool visible() const { return m_visible; }

    void draw(DrawTarget& target);
    bool pointerDown(FCoords coords) override;
    bool pointerMove(FCoords coords) override;
    bool pointerUp(bool empty) override;
    void pointerCancel() override;
    bool doubleTap(FCoords coords) override;
    bool keyDown(Key key) override;

private:
    Instance& m_instance;
    ImageRef m_options;
    ImageRef m_slider;
    FCoords m_origin;
    bool m_visible;

    struct Button {
        std::string value;
        ImageRef image;
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

    static float exp(float x);
    static float log(float v);

    constexpr static FCoords imgSize{195, 332};
    constexpr static int shadowBorder = 20;
    constexpr static float volLength = 120.f;
    constexpr static float volSliderOffset = 8.f;
    constexpr static float volTolerance = 10.f;
    constexpr static float volSpan = 63.f;
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H
