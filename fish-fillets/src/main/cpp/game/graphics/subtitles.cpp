#include "subtitles.h"

Subtitles::Subtitles(Instance& instance) :
    m_instance(instance),
    m_font(decoders::ttf(instance, fontFilename))
{ }

void Subtitles::defineColors(const std::string& name, Color color1, Color color2) {
    m_colors.insert({name, {color1, color2}});
}

void Subtitles::add(const std::string& text, const std::string& colors) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    auto lines = m_font->breakLines(text, Graphics::baseDim.fx() * coords.scale);
    auto countLines = lines.size();
    for(const auto& line : lines) {
        auto duration = std::max((int)text.length() * timePerChar, minTimePerLine);
        auto [color1, color2] = [&]() {
            if(m_colors.contains(colors))
                return m_colors.at(colors);
            else {
                Log::warn("Unknown color: ", colors);
                return std::pair{Color::white, Color::white};
            }
        }();
        m_lines.push_back({
                TextImage(m_instance, *m_font, line),
                false, 0.f, 0.f, duration,
                (unsigned)countLines, color1, color2
        });
    }
}

void Subtitles::clear() {
    m_lines.clear();
}

void Subtitles::update(float absTime, float dTime) {
    auto liveEnd = std::find_if(m_lines.begin(), m_lines.end(), [](const auto& line) { return !line.live; });
    float lowest = std::accumulate(m_lines.begin(), liveEnd, 0.f, [](float y, const auto& line) { return std::min(y, line.yOffset); });
    float dy = std::min(dTime * speed, -lowest);
    for(auto& line : m_lines)
        line.yOffset += dy;
    if(lowest >= -0.5f && liveEnd != m_lines.end()) {
        auto& line = *liveEnd;
        line.live = true;
        line.yOffset = -1.5f;
        line.addTime = absTime;
        Log::verbose<Log::graphics>("subtitle live: ", line.image.text());
    }
    while(!m_lines.empty()) {
        const auto& front = m_lines.front();
        if(front.live && ((front.yOffset > 5 && m_lines.front().groupSize < m_lines.size()) || absTime - front.addTime > front.duration.count())) {
            Log::verbose<Log::graphics>("hiding old subtitle: ", m_lines.front().groupSize, " lines");
            m_lines.erase(m_lines.begin(), m_lines.begin() + (int)m_lines.front().groupSize);
        } else
            break;
    }
}

void Subtitles::draw(DrawTarget& target, float time) {
    if(m_lines.empty())
        return;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    auto bottomY = coords.out2in(FCoords{0.f, m_instance.graphics().display().size().fy()}).fy();
    for(const auto& line : m_lines)
        if(line.live) {
            const auto program = m_instance.graphics().shaders().wavyText({
                .color1 = line.color1,
                .color2 = line.color2,
                .time = time - line.addTime
            });
            auto size = line.image.size() / coords.scale;
            FCoords dest0{320.f - size.fx() / 2.f, bottomY - size.fy() * (2.5f + line.yOffset)};
            FCoords dest = coords.out2in(coords.in2out(dest0).round());
            target.draw(line.image.texture(), program, coords, { .dest = dest, .area = FCoords{size.fx(), 3.f * size.fy()} });
        }
}

void Subtitles::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    m_font->setSizes(fontSize * coords.scale, outline * coords.scale);
    for(auto& line : m_lines)
        line.image.render();
}
