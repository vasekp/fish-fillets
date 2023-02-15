#include "leveltitle.h"

LevelTitle::LevelTitle(Instance& instance) :
    m_instance(instance),
    m_font(decoders::ttf(instance, LevelTitle::fontFilename))
{ }

void LevelTitle::set(const std::string& text) {
    m_image.emplace(m_instance, *m_font, text);
}

void LevelTitle::reset() {
    m_image.reset();
}

void LevelTitle::draw(const DrawTarget& target, float opacity) {
    if(!m_image)
        return;

    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto& program = m_instance.graphics().shaders().titleText;

    FCoords baseDim{640, 480};
    FCoords offset{baseDim.fx() / 2.f - baseDim.fy() * maxWidth / 2.f, startY * baseDim.fy()};
    FCoords rect{baseDim.fy() * maxWidth, (endY - startY) * baseDim.fy()};
    glUseProgram(program);
    glUniform2f(program.uniform("uBlitSize"), rect.fx(), rect.fy());

    const FCoords shadow{2.f, 2.f}; // TODO constexpr
    glUniform4fv(program.uniform("uColor"), 1, colorBg.gl(opacity).data());
    target.blit(m_image->texture(), coords, program, offset.fx() + shadow.fx(), offset.fy() + shadow.fy(), 0, 0, rect.x(), rect.y());

    glUniform4fv(program.uniform("uColor"), 1, colorFg.gl(opacity).data());
    target.blit(m_image->texture(), coords, program, offset.fx(), offset.fy(), 0, 0, rect.x(), rect.y());
}

void LevelTitle::refresh() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    Log::debug("title ", coords.scale);
    m_font->setSizes(fontSize * coords.scale, 0.f);
    if(m_image)
        m_image->render();
}