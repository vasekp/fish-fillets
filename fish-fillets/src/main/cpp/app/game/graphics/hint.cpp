#include "hint.h"

Hint::Hint(Instance& instance, const std::string& text) :
    m_instance(instance),
    m_font(decoders::ttf(instance, fontFilename)),
    m_lines()
{
    resize();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    auto lines = m_font->breakLines(text, coords.size.fx() * coords.scale);
    for(const auto& line : lines)
        m_lines.push_back(TextImage(m_instance, *m_font, line));
}

void Hint::draw(DrawTarget& target) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::window);
    const auto program = m_instance.graphics().shaders().copy();
    float y = 0;
    for(const auto& line : m_lines) {
        auto size = line.size() / coords.scale;
        FCoords dest0{(coords.size.fx() - size.fx()) / 2.f, y};
        FCoords dest = coords.out2in(coords.in2out(dest0).round());
        target.draw(line.texture(), program, coords, { .dest = dest, .srcSize = size });
        y += size.fy();
    }
}

void Hint::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    m_font->setSizes(fontSize * coords.scale, outline * coords.scale);
    for(auto& line : m_lines)
        line.render();
}
