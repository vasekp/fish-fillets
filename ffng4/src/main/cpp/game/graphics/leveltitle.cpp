#include "leveltitle.h"

LevelTitle::LevelTitle(Instance& instance, IFont& font, std::string text) :
    m_instance(instance),
    m_image(instance, font, std::move(text))
{ }

void LevelTitle::draw(const DrawTarget& target, float opacity) {
    auto& display = m_instance.graphics().display();
    const auto& program = m_instance.graphics().shaders().titleText;

    FCoords dims{display.width(), display.height()};
    FCoords offset{dims.fx() / 2.f - dims.fy() * maxWidth / 2.f, startY * dims.fy()};
    FCoords rect{dims.fy() * maxWidth, (endY - startY) * dims.fy()};
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());

    float shift = 2.f / 480.f * dims.fy();
    glUniform4fv(program.uniform("uColor"), 1, colorBg.gl(opacity).data());
    target.blit(m_image.texture(), program, offset.fx() + shift, offset.fy() + shift, 0, 0, rect.x(), rect.y());

    glUniform4fv(program.uniform("uColor"), 1, colorFg.gl(opacity).data());
    target.blit(m_image.texture(), program, offset.fx(), offset.fy(), 0, 0, rect.x(), rect.y());
}
