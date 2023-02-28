#include "pedometer.h"

Pedometer::Pedometer(Instance& instance):
    m_instance(instance),
    m_pmImage{instance, "images/menu/pedometer.png"},
    m_buttons{
        {Buttons::retry, {instance, "images/menu/pm-retry.png"}, {259, 223}},
        {Buttons::replay, {instance, "images/menu/pm-replay.png"}, {301, 223}},
        {Buttons::close, {instance, "images/menu/pm-close.png"}, {343, 223}}
    }
{ }

void Pedometer::draw(const DrawTarget& target, FCoords hover) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.blit(&m_pmImage, coords, copyProgram, pos.x, pos.y);
    const auto& button = m_buttons[1];
    target.blit(&button.image, coords, copyProgram, button.origin.x, button.origin.y);
}

Pedometer::Buttons Pedometer::findButton(FCoords coords) {
    for(const auto& button : m_buttons)
        if(coords.within(button.origin, button.origin + Button::size))
            return button.type;
    return Buttons::none;
}
