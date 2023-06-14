#include "screenmanager.h"
#include "subsystem/persist.h"

OptionsOverlay::OptionsOverlay(Instance& instance) :
    GameScreen(instance),
    m_options(instance, "images/menu/options.png"),
    m_slider(instance, "images/menu/cudlik.png"),
    m_origin((Graphics::baseDim - imgSize) / 2),
    m_visible(false),
    m_buttons{
        {Subtitles::cz, {instance, "images/menu/options-subs-cz.png"}, {25, 270}},
        {Subtitles::en, {instance, "images/menu/options-subs-en.png"}, {72, 270}},
        {Subtitles::none, {instance, "images/menu/options-subs-none.png"}, {120, 270}}},
    m_volbars{
        {AudioType::sound, {37, 105}},
        {AudioType::talk, {37, 154}},
        {AudioType::music,  {37, 203}}},
    m_sliding(nullptr),
    m_subs(Subtitles::cz)
{ }

OptionsOverlay::VolumeBar::VolumeBar(AudioType type_, FCoords origin_) :
    type(type_), origin(origin_)
{
    static int i = 0;
    from = origin - FCoords(volTolerance, volTolerance);
    to = origin + FCoords(volTolerance, volTolerance) + FCoords(volLength, 0.f);
}

void OptionsOverlay::own_draw(const DrawTarget& target, float dt) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    auto coords = m_instance.graphics().coords(Graphics::CoordSystems::base).shifted(m_origin);

    target.blit(&m_options, coords, copyProgram, 0, 0);

    for(const auto& button : m_buttons)
        if(button.value == m_subs)
            target.blit(&button.image, coords, copyProgram, button.origin.fx(), button.origin.fy());

    for(const auto& bar : m_volbars)
        target.blit(&m_slider, coords, copyProgram, bar.origin.fx() + log(m_instance.audio().getVolume(bar.type)) * volLength - volSliderOffset, bar.origin.fy() - volSliderOffset);
}

void OptionsOverlay::show() {
    m_visible = true;
    m_instance.inputSource().reset();
    int x = m_instance.persist().get<int>("pokus", 3);
    Log::info("pokus = ", x);
    x++;
    m_instance.persist().set("pokus", x);
}

void OptionsOverlay::hide() {
    m_visible = false;
    pointerCancel();
    m_instance.inputSource().reset();
}

bool OptionsOverlay::pointerDown(FCoords coords) {
    auto lcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(coords) - m_origin;
    m_sliding = nullptr;
    if(!lcoords.within({0, 0}, imgSize)) {
        hide();
        return true;
    }
    for(const auto& bar : m_volbars)
        if(lcoords.within(bar.from, bar.to)) {
            m_sliding = &bar;
            float volume = exp(std::clamp((lcoords.fx() - bar.origin.fx()) / volLength, 0.f, 1.f));
            m_instance.audio().setVolume(bar.type, volume);
            return false;
        }
    for(const auto& button : m_buttons)
        if(lcoords.within(button.origin, button.origin + Button::size)) {
            m_subs = button.value;
            return true;
        }
    return false;
}

bool OptionsOverlay::pointerMove(FCoords coords) {
    if(!m_sliding)
        return false;
    auto& bar = *m_sliding;
    auto lcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(coords) - m_origin;
    float volume =exp(std::clamp((lcoords.fx() - bar.origin.fx()) / volLength, 0.f, 1.f));
    m_instance.audio().setVolume(bar.type, volume);
    return true;
}

bool OptionsOverlay::pointerUp(bool empty) {
    m_sliding = nullptr;
    return false;
}

void OptionsOverlay::pointerCancel() {
    m_sliding = nullptr;
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