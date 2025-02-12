#include "subtitles.h"
#include "subsystem/graphics.h"

static constexpr const char* fontFilename = "font/font_subtitle.ttf";
static constexpr float fontSize = 20.f; // in "reduced base pixels": 640x480 window after reducing button strip
static constexpr float outline = 2.f;
static constexpr auto timePerChar = 90ms;
static constexpr auto minTimePerLine = 2500ms;
static constexpr auto newLineTime = 1s;

Subtitles::Subtitles(Instance& instance) :
    m_instance(instance),
    m_font(decoders::ttf(instance, fontFilename)),
    m_lastUpdate()
{ }

void Subtitles::defineColors(const std::string& name, Color color1, Color color2) {
    m_colors.insert({name, {color1, color2}});
}

void Subtitles::add(const std::string& text, const std::string& colors) {
    auto lines = m_font->breakLines(text, Graphics::baseDim.x);
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
                TextImage::create(m_instance, *m_font, line),
                false, 0.f, {}, duration,
                (unsigned)countLines, color1, color2
        });
    }
}

void Subtitles::clear() {
    m_lines.clear();
}

void Subtitles::update(LiveClock::time_point time) {
    auto dt = time - m_lastUpdate;
    auto liveEnd = std::find_if(m_lines.begin(), m_lines.end(), [](const auto& line) { return !line.live; });
    float lowest = std::accumulate(m_lines.begin(), liveEnd, 0.f, [](float y, const auto& line) { return std::min(y, line.yOffset); });
    float dy = std::min(dt / newLineTime, -lowest);
    for(auto& line : m_lines)
        line.yOffset += dy;
    if(lowest >= -0.5f && liveEnd != m_lines.end()) {
        auto& line = *liveEnd;
        line.live = true;
        line.yOffset = -1.5f;
        line.addTime = time;
    }
    while(!m_lines.empty()) {
        const auto& front = m_lines.front();
        if(front.live && ((front.yOffset > 5 && m_lines.front().groupSize < m_lines.size()) || time - front.addTime > front.duration)) {
            Log::verbose<Log::graphics>("hiding old subtitle: ", m_lines.front().groupSize, " lines");
            m_lines.erase(m_lines.begin(), m_lines.begin() + (int)m_lines.front().groupSize);
        } else
            break;
    }
    m_lastUpdate = time;
}

void Subtitles::draw(DrawTarget& target, LiveClock::time_point time) {
    if(m_lines.empty())
        return;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    auto bottomY = coords.out2in(target.size()).y;
    for(const auto& line : m_lines)
        if(line.live) {
            const auto program = m_instance.graphics().shaders().wavyText({
                .color1 = line.color1.gl(),
                .color2 = line.color2.gl(),
                .time = (time - line.addTime).count()
            });
            auto size = line.image->size();
            FCoords dest{320.f - size.x / 2.f, bottomY - size.y * (2.5f + line.yOffset)};
            target.draw(line.image, program, coords, {
                .dest = coords.pixelAlign(dest),
                .area = FCoords{size.x, 3.f * size.y}
            });
        }
}

void Subtitles::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::reduced);
    m_font->setSizes(fontSize, outline, coords.scale);
}
