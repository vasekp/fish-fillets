#include "title.h"

void LevelTitle::refresh() {
    m_texture = m_instance.graphics().renderText(m_text, "font/font_title.ttf", 16.0, 0.0, m_instance.graphics().fullscreenTarget());
}

void LevelTitle::draw(const DrawTarget& target, float opacity) {
    auto& display = m_instance.graphics().display();
    const auto& program = m_instance.graphics().shaders().titleText;

    Coords dims{display.width(), display.height()};
    Coords offset{dims.fx() / 2.f - dims.fy() * maxWidth / 2.f, startY * dims.fy()};
    Coords rect{dims.fy() * maxWidth, (endY - startY) * dims.fy()};
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());

    auto shift = (int)(2.f / 480.f * dims.fy());
    glUniform4f(program.uniform("uColor"), colorBg.rf(), colorBg.rg(), colorBg.rb(), opacity);
    target.blit(m_texture, program, offset.x() + shift, offset.y() + shift, 0, 0, rect.x(), rect.y());

    glUniform4f(program.uniform("uColor"), colorFg.rf(), colorFg.rg(), colorFg.rb(), opacity);
    target.blit(m_texture, program, offset.x(), offset.y(), 0, 0, rect.x(), rect.y());
}