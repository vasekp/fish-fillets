#include "pedometer.h"
#include "subsystem/files.h"
#include "subsystem/graphics.h"
#include "subsystem/input.h"
#include "screenmanager.h"

static constexpr ICoords pos{193, 141};
static constexpr FCoords digitArray{275, 177};
static constexpr FCoords digitSize{19, 24};
static constexpr auto digitTime = 250ms;

constexpr FCoords offset{0.5f, 0.5f};

Pedometer::Pedometer(Instance& instance, LevelRecord& level, LiveClock::time_point time):
        m_instance(instance),
        m_record(level),
        m_pmImage(PNGImage::create(instance, "images/menu/pedometer.png")),
        m_digImage(PNGImage::create(instance, "images/menu/numbers.png")),
        m_digits{},
        m_createTime(time),
        m_buttons{
        {Buttons::retry, PNGImage::create(instance, "images/menu/pm-retry.png"), {259, 223}},
        {Buttons::replay, PNGImage::create(instance, "images/menu/pm-replay.png"), {301, 223}},
        {Buttons::close, PNGImage::create(instance, "images/menu/pm-close.png"), {343, 223}}
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

void Pedometer::draw(DrawTarget& target, LiveClock::time_point time) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    target.draw(m_pmImage, copyProgram, coords, { .dest = FCoords{pos} });
    if(auto hover = m_instance.inputSourceMasked().hover(); hover != IInputSource::noHover) {
        auto hcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(hover);
        for(const auto& button : m_buttons)
            if(hcoords.within(button.origin + offset, button.origin + Button::size - offset))
                target.draw(button.image, copyProgram, coords, { .dest = button.origin });
    }
    float yBase = (time - m_createTime) / digitTime;
    for(auto x = 0u; x < m_digits.size(); x++) {
        float y = std::min((float)m_digits[x], yBase);
        target.draw(m_digImage, copyProgram, coords, {
           .src = FCoords{0.f, (9.f - y) * digitSize.y},
           .dest = digitArray + FCoords{x * digitSize.x, 0.f},
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
