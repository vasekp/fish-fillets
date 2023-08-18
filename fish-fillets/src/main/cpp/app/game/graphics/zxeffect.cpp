#include "zxeffect.h"
#include "subsystem/graphics.h"
#include "subsystem/rng.h"

static constexpr float height1 = 38.5;
static constexpr float height2 = 3.4;
static constexpr float roomHeight = 36 * 15.f;

static constexpr std::array<std::pair<Color, Color>, 2> colors{{
    {0x800000, 0x008080},
    {0x000080, 0x808000}
}};

ZXEffect::ZXEffect(Instance& instance) :
    m_instance(instance),
    m_stripHeight(height1),
    m_stripOffset(0),
    m_colors(0),
    m_lastTick(0)
{
    update(0);
}

void ZXEffect::render(DrawTarget& target, const Image* image) {
    const auto program = m_instance.graphics().shaders().ZX({
        .color1 = colors[m_colors].first.gl(),
        .color2 = colors[m_colors].second.gl(),
        .period = 2.f * m_stripHeight,
        .offset = m_stripOffset
    });
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    target.draw(image, program, coords);
}

void ZXEffect::update(int tickCount) {
    if(tickCount == m_lastTick)
        return;
    int phase = tickCount % 500;
    if(phase == 1) {
        m_colors = 0;
        m_stripHeight = height1;
    } else if(phase < 52) {
        m_stripHeight = (3.f * m_stripHeight * m_instance.rng().randomFloat(0.97, 1.03) + height1) / 4.f;
    } else if(phase == 52) {
        m_colors = 1;
        m_stripHeight = height2;
    } else {
        m_stripHeight = (3.f * m_stripHeight * m_instance.rng().randomFloat(0.95, 1.05) + height2) / 4.f;
    }
    m_lastTick = tickCount;
    m_stripOffset = std::fmod(m_stripOffset + roomHeight, 2.f * m_stripHeight);
}
