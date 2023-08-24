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
    void pointerMove(FCoords coords) override;
    bool pointerUp() override;
    void pointerCancel() override;
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

    bool slider(FCoords coords);
    static float exp(float x);
    static float log(float v);
};

#endif //FISH_FILLETS_SCREENS_OPTIONS_H
