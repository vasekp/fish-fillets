#include "pedometer.h"

Pedometer::Pedometer(Instance& instance):
    m_instance(instance),
    m_pmImage{instance, "images/menu/pedometer.png"},
    m_digImage{instance, "images/menu/numbers.png"},
    m_digits{{0, 1, 5, 3, 7}},
    m_time(-1),
    m_buttons{
        {Buttons::retry, {instance, "images/menu/pm-retry.png"}, {259, 223}},
        {Buttons::replay, {instance, "images/menu/pm-replay.png"}, {301, 223}},
        {Buttons::close, {instance, "images/menu/pm-close.png"}, {343, 223}}
    }
{ }

void Pedometer::draw(const DrawTarget& target, float dt, FCoords hover) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.blit(&m_pmImage, coords, copyProgram, pos.x, pos.y);
    const auto& button = m_buttons[1];
    target.blit(&button.image, coords, copyProgram, button.origin.x, button.origin.y);
    if(m_time < 0)
        m_time = 0; // first call
    else
        m_time += dt;
    float yBase = m_time * digitSpeed;
    for(auto x = 0u; x < m_digits.size(); x++) {
        float y = std::min(m_digits[x], yBase);
        target.blit(&m_digImage, coords, copyProgram, digitArray.x + x * digitSize.x, digitArray.y, 0, (9.f - y) * digitSize.y, digitSize.x, digitSize.y);
    }
}

Pedometer::Buttons Pedometer::findButton(FCoords coords) {
    for(const auto& button : m_buttons)
        if(coords.within(button.origin, button.origin + Button::size))
            return button.type;
    return Buttons::none;
}
