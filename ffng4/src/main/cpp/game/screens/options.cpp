#include "screenmanager.h"

OptionsOverlay::OptionsOverlay(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_options(instance, "images/menu/options.png"),
    m_slider(instance, "images/menu/cudlik.png"),
    m_origin((Graphics::baseDim - imgSize) / 2),
    m_visible(false),
    m_buttons{
        {Subtitles::cz, {instance, "images/menu/options-subs-cz.png"}, {25, 270}},
        {Subtitles::en, {instance, "images/menu/options-subs-en.png"}, {72, 270}},
        {Subtitles::none, {instance, "images/menu/options-subs-none.png"}, {120, 270}}},
    m_volbars{
        {Volumes::sound, {29, 97}},
        {Volumes::music, {29, 146}},
        {Volumes::talk, {29, 195}}}
{ }

void OptionsOverlay::own_draw(const DrawTarget& target, float dt) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    auto coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    coords.origin += m_origin;

    target.blit(&m_options, coords, copyProgram, 0, 0);
    const auto& button = m_buttons[0];
    target.blit(&button.image, coords, copyProgram, button.origin.fx(), button.origin.fy());

    for(const auto& bar : m_volbars)
        target.blit(&m_slider, coords, copyProgram, bar.origin.fx() + ((int)bar.type)/2.f * volLength, bar.origin.fy());
}

void OptionsOverlay::show() {
    m_visible = true;
    m_instance.inputSource().reset();
}

void OptionsOverlay::hide() {
    m_visible = false;
    m_instance.inputSource().reset();
}

bool OptionsOverlay::own_pointer(FCoords coords, bool longPress) {
    hide();
    return true;
}