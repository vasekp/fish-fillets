#include "screenmanager.h"

OptionsOverlay::OptionsOverlay(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_options1(instance, "images/menu/options.png"),
    m_options2(instance, "images/menu/options-over.png"),
    m_origin((Graphics::baseDim - imgSize) / 2),
    m_visible(false)
{ }

void OptionsOverlay::own_draw(const DrawTarget& target, float dt) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);

    target.blit(&m_options1, coords, copyProgram, m_origin.fx(), m_origin.fy());
    /*if(auto hover = m_instance.inputSource().hover(); hover != IInputSource::noHover) {
        auto hcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(hover);
        for(const auto& button : m_buttons)
            if(hcoords.within(button.origin, button.origin + Button::size))
                target.blit(&button.image, coords, copyProgram, button.origin.fx(), button.origin.fy());
    }
    if(m_time < 0)
        m_time = 0; // first call
    else
        m_time += dt;
    float yBase = m_time * digitSpeed;
    for(auto x = 0u; x < m_digits.size(); x++) {
        float y = std::min((float)m_digits[x], yBase);
        target.blit(&m_digImage, coords, copyProgram, digitArray.fx() + (float)x * digitSize.fx(), digitArray.fy(), 0, (9.f - y) * digitSize.fy(), digitSize.x(), digitSize.y());
    }*/
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