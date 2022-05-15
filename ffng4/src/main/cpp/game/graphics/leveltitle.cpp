#include "leveltitle.h"

void LevelTitle::refresh() {
    m_texture = m_instance.graphics().renderText(m_text, "font/font_title.ttf", 16.0, 0.0);
}

void LevelTitle::draw(const DrawTarget& target, float opacity) {
    auto& display = m_instance.graphics().display();
    const auto& program = m_instance.graphics().shaders().titleText;

    FCoords dims{display.width(), display.height()};
    FCoords offset{dims.fx() / 2.f - dims.fy() * maxWidth / 2.f, startY * dims.fy()};
    FCoords rect{dims.fy() * maxWidth, (endY - startY) * dims.fy()};
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());

    float shift = 2.f / 480.f * dims.fy();
    glUniform4f(program.uniform("uColor"), colorBg.rf(), colorBg.rg(), colorBg.rb(), opacity);
    target.blit(m_texture, program, offset.fx() + shift, offset.fy() + shift, 0, 0, rect.x(), rect.y());

    glUniform4f(program.uniform("uColor"), colorFg.rf(), colorFg.rg(), colorFg.rb(), opacity);
    target.blit(m_texture, program, offset.fx(), offset.fy(), 0, 0, rect.x(), rect.y());
}