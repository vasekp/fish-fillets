#include "leveltitle.h"

LevelTitle::LevelTitle(Instance& instance) :
    m_instance(instance),
    m_font(decoders::ttf(instance, LevelTitle::fontFilename))
{ }

void LevelTitle::show(const std::string& text) {
    m_image.emplace(m_instance, *m_font, text);
    m_hide.reset();
}

void LevelTitle::hide() {
    m_image.reset();
    m_hide.reset();
}

void LevelTitle::fadeout() {
    m_hide = std::chrono::steady_clock::now() + lingerTime + fadeoutTime;
}

void LevelTitle::update() {
    if(m_hide) {
        std::chrono::duration<float> timeLeft = m_hide.value() - std::chrono::steady_clock::now();
        m_opacity = std::clamp(timeLeft / fadeoutTime, 0.f, 1.f);
    } else
        m_opacity = 1.f;
    if(m_opacity == 0.f) {
        hide();
        return;
    }
}

void LevelTitle::draw(DrawTarget& target) {
    if(!m_image)
        return;

    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto& program = m_instance.graphics().shaders().titleText;

    constexpr FCoords offset{Graphics::baseDim.fx() / 2.f - maxWidth / 2.f, startY};
    constexpr FCoords rect{maxWidth, endY - startY};
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());
    glUniform2f(program.uniform("uSrcSizeScaled"), (float)m_image->texture().width() / coords.scale, (float)m_image->texture().height() / coords.scale);

    glUniform4fv(program.uniform("uColor"), 1, colorBg.gl(m_opacity).data());
    target.blit(m_image->texture(), coords, program, offset.fx() + shadow.fx(), offset.fy() + shadow.fy(), 0, 0, rect.x(), rect.y());

    glUniform4fv(program.uniform("uColor"), 1, colorFg.gl(m_opacity).data());
    target.blit(m_image->texture(), coords, program, offset.fx(), offset.fy(), 0, 0, rect.x(), rect.y());
}

void LevelTitle::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    m_font->setSizes(fontSize * coords.scale, 0.f);
    if(m_image)
        m_image->render();
}
