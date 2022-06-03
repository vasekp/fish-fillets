#include "subsystem/graphics.h"
#include "zxeffect.h"

ZXEffect::ZXEffect(Instance& instance) :
    m_instance(instance),
    m_stripHeight(height1),
    m_stripOffset(0),
    m_colors(0),
    m_lastTick(0)
{
    update(0);
}

void ZXEffect::render(const DrawTarget& target, const Image* image) {
    const auto& program = m_instance.graphics().shaders().zx;
    glUseProgram(program);
    glUniform4fv(program.uniform("uColor1"), 1, colors[m_colors].first.gl().get());
    glUniform4fv(program.uniform("uColor2"), 1, colors[m_colors].second.gl().get());
    glUniform1f(program.uniform("uPeriod"), 2.f * m_stripHeight);
    glUniform1f(program.uniform("uOffset"), m_stripOffset);
    target.blit(image, program);
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