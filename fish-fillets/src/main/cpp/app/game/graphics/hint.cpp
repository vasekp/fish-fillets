#include "hint.h"
#include "subsystem/graphics.h"

static constexpr const char* fontFilename = "font/font_subtitle.ttf";
static constexpr float fontSize = 20.f; // in "reduced base pixels": 640x480 window after reducing button strip
static constexpr float outline = 2.f;

Hint::Hint(Instance& instance, const std::string& text, bool fullWidth) :
    m_instance(instance),
    m_font(decoders::ttf(instance, fontFilename)),
    m_lines(),
    m_coords(fullWidth ? Graphics::CoordSystems::base : Graphics::CoordSystems::window)
{
    resize();
    setText(text);
}

void Hint::draw(DrawTarget& target) {
    const auto& coords = m_instance.graphics().coords(m_coords);
    const auto program = m_instance.graphics().shaders().copy();
    float y = 0;
    for(const auto& line : m_lines) {
        auto size = line->size();
        FCoords dest{(coords.size.x - size.x) / 2.f, y};
        target.draw(line, program, coords, { .dest = coords.pixelAlign(dest) });
        y += size.y;
    }
}

void Hint::setText(const std::string& text) {
    const auto& coords = m_instance.graphics().coords(m_coords);
    auto lines = m_font->breakLines(text, coords.size.x);
    m_lines.clear();
    for(const auto& line : lines)
        m_lines.push_back(TextImage::create(m_instance, *m_font, line));
}

void Hint::resize() {
    const auto& coords = m_instance.graphics().coords(m_coords);
    m_font->setSizes(fontSize, outline, coords.scale);
}
