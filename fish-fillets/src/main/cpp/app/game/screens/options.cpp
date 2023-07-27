#include "options.h"
#include "subsystem/persist.h"

OptionsOverlay::OptionsOverlay(Instance& instance) :
    m_instance(instance),
    m_options(instance, "images/menu/options.png"),
    m_slider(instance, "images/menu/cudlik.png"),
    m_origin((Graphics::baseDim - imgSize) / 2),
    m_visible(false),
    m_buttons{
        {"cs"s, {instance, "images/menu/options-subs-cs.png"}, {25, 270}},
        {"en"s, {instance, "images/menu/options-subs-en.png"}, {72, 270}},
        {""s, {instance, "images/menu/options-subs-none.png"}, {120, 270}}},
    m_volbars{
        {AudioType::sound, "sound", {37, 105}},
        {AudioType::talk, "talk", {37, 154}},
        {AudioType::music, "music", {37, 203}}},
    m_sliding(nullptr)
{ }

OptionsOverlay::VolumeBar::VolumeBar(AudioType type_, const char* typeString_, FCoords origin_) :
    type(type_), typeString(typeString_), origin(origin_)
{
    static int i = 0;
    from = origin - FCoords(volTolerance, volTolerance);
    to = origin + FCoords(volTolerance, volTolerance) + FCoords(volLength, 0.f);
}

void OptionsOverlay::draw(DrawTarget& target) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    auto coords = m_instance.graphics().coords(Graphics::CoordSystems::base).shifted(m_origin);

    target.draw(&m_options, copyProgram, coords);

    for(const auto& button : m_buttons)
        if(button.value == m_currSubs)
            target.draw(&button.image, copyProgram, coords, { .dest = button.origin });

    for(const auto& bar : m_volbars)
        target.draw(&m_slider, copyProgram, coords, {
            .dest = FCoords{bar.origin.fx() + log(m_instance.audio().getVolume(bar.type)) * volLength - volSliderOffset, bar.origin.fy() - volSliderOffset}
        });
}

void OptionsOverlay::show() {
    m_visible = true;
    m_instance.inputSource().reset();
    m_currSubs = m_instance.persist().get("subtitles", "cs"s);
}

void OptionsOverlay::hide() {
    m_visible = false;
    pointerCancel();
    m_instance.inputSource().reset();
}

bool OptionsOverlay::pointerDown(FCoords coords) {
    auto lcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(coords) - m_origin;
    m_sliding = nullptr;
    constexpr FCoords from = {shadowBorder, shadowBorder};
    constexpr FCoords to = imgSize - FCoords{shadowBorder, shadowBorder};
    if(!lcoords.within(from, to)) {
        hide();
        return true;
    }
    for(const auto& bar : m_volbars)
        if(lcoords.within(bar.from, bar.to)) {
            m_sliding = &bar;
            return pointerMove(coords);
        }
    for(const auto& button : m_buttons)
        if(lcoords.within(button.origin, button.origin + Button::size)) {
            m_currSubs = button.value;
            m_instance.persist().set("subtitles", m_currSubs);
            return true;
        }
    return false;
}

bool OptionsOverlay::pointerMove(FCoords coords) {
    if(!m_sliding)
        return false;
    const auto& bar = *m_sliding;
    auto lcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(coords) - m_origin;
    float volume = exp(std::clamp((lcoords.fx() - bar.origin.fx()) / volLength, 0.f, 1.f));
    m_instance.audio().setVolume(bar.type, volume);
    m_instance.persist().set("volume_"s + bar.typeString, (int)(volume * 100));
    return true;
}

bool OptionsOverlay::pointerUp(bool empty) {
    m_sliding = nullptr;
    return false;
}

void OptionsOverlay::pointerCancel() {
    m_sliding = nullptr;
}

bool OptionsOverlay::doubleTap(FCoords coords) {
    return pointerDown(coords);
}

bool OptionsOverlay::keyDown(Key key) {
    hide();
    return true;
}

float OptionsOverlay::exp(float x) {
    return (std::pow(volSpan + 1.f, x) - 1.f) / volSpan;
}

float OptionsOverlay::log(float v) {
    return std::log(volSpan * v + 1.f) / std::log(volSpan + 1.f);
}