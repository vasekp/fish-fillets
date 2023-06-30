#include "hint.h"

Hint::Hint(Instance& instance, const std::string& text) :
    m_instance(instance),
    m_font(decoders::ttf(instance, fontFilename)),
    m_lines()
{
    resize();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    auto lines = m_font->breakLines(text, coords.size.fx() * coords.scale);
    for(const auto& line : lines) {
        Log::debug(line);
        m_lines.push_back(TextImage(m_instance, *m_font, line));
    }
}

void Hint::draw(const DrawTarget& target) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    const auto& program = m_instance.graphics().shaders().copy;
    float y = 0;
    for(const auto& line : m_lines) {
        auto width = (float)line.width() / coords.scale;
        auto height = (float)line.height() / coords.scale;
        FCoords dest0{(coords.size.fx() - width) / 2.f, y};
        FCoords dest = coords.out2in(coords.in2out(dest0).round());
        Log::debug("drawing hint ", width, "x", height, " at ", dest);
        line.texture().bind();
        target.fill(coords, program, dest.fx(), dest.fy(), dest.fx() + (int)width, dest.fy() + (int)height);
        y += height;
    }
}

void Hint::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    m_font->setSizes(fontSize * coords.scale, outline * coords.scale);
    for(auto& line : m_lines)
        line.render();
}
