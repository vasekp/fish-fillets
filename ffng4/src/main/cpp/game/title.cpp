#include "title.h"

void LevelTitle::refresh() {
    m_texture = m_instance.graphics().renderText("How it all started", "font/font_title.ttf", 16.0, 0.0, m_instance.graphics().fullscreenTarget());
}

void LevelTitle::draw() {
    auto& target = m_instance.graphics().fullscreenTarget();
    auto& display = m_instance.graphics().display();
    const auto& program = m_instance.graphics().shaders().titleText;

    Coords dims{display.width(), display.height()};
    Coords offset{dims.fx() / 2.f - dims.fy() / 3.f, startY * dims.fy()};
    Coords rect{dims.fy() * 2.f / 3.f, (endY - startY) * dims.fy()};
    target.bind();
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());

    auto shift = (int)(2.f / 480.f * dims.fy());
    glUniform4fv(program.uniform("uColor"), 1, colorBg.gl().get());
    target.blit(m_texture, program, offset.x() + shift, offset.y() + shift, 0, 0, rect.x(), rect.y());

    glUniform4fv(program.uniform("uColor"), 1, colorFg.gl().get());
    target.blit(m_texture, program, offset.x(), offset.y(), 0, 0, rect.x(), rect.y());
}