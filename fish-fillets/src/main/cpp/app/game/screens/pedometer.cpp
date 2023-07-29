#include "pedometer.h"
#include "subsystem/files.h"
#include "subsystem/input.h"
#include "screenmanager.h"

constexpr FCoords offset{0.5f, 0.5f};

Pedometer::Pedometer(Instance& instance, LevelRecord& level, float time):
        m_instance(instance),
        m_record(level),
        m_pmImage{instance, "images/menu/pedometer.png"},
        m_digImage{instance, "images/menu/numbers.png"},
        m_digits{},
        m_createTime(time),
        m_buttons{
        {Buttons::retry, {instance, "images/menu/pm-retry.png"}, {259, 223}},
        {Buttons::replay, {instance, "images/menu/pm-replay.png"}, {301, 223}},
        {Buttons::close, {instance, "images/menu/pm-close.png"}, {343, 223}}
    }
{
    auto solve = m_instance.files().user(m_record.solveFilename())->read();
    auto steps = [&]() {
        auto i = solve.find('\'');
        auto j = solve.find('\'', i + 1);
        if(i == std::string::npos || j == std::string::npos)
            Log::error("Solve file does not contain '-delimited string: ", m_record.solveFilename());
        return int(j - i - 1);
    }();
    for(auto i = m_digits.size(); i > 0; i--) {
        m_digits[i - 1] = steps % 10;
        steps /= 10;
    }
}

void Pedometer::draw(DrawTarget& target, float time) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.draw(&m_pmImage, copyProgram, coords, { .dest = pos });
    if(auto hover = m_instance.inputSourceMasked().hover(); hover != IInputSource::noHover) {
        auto hcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(hover);
        for(const auto& button : m_buttons)
            if(hcoords.within(button.origin + offset, button.origin + Button::size - offset))
                target.draw(&button.image, copyProgram, coords, { .dest = button.origin });
    }
    float yBase = (time - m_createTime) * digitSpeed;
    for(auto x = 0u; x < m_digits.size(); x++) {
        float y = std::min((float)m_digits[x], yBase);
        target.draw(&m_digImage, copyProgram, coords, {
           .src = FCoords{0.f, (9.f - y) * digitSize.fy()},
           .dest = digitArray + FCoords{x * digitSize.fx(), 0.f},
           .area = digitSize
        });
    }
}

Pedometer::Buttons Pedometer::findButton(FCoords coords) {
    for(const auto& button : m_buttons)
        if(coords.within(button.origin + offset, button.origin + Button::size - offset))
            return button.type;
    return Buttons::none;
}
