#include "subtitles.h"

void Subtitles::add(const std::string &text, Color color, float addTime) {
    auto textures = m_instance.graphics().renderMultiline(text);
    auto countLines = textures.size();
    for(auto& texture : textures) {
        float startY = (m_lines.empty() ? 0.f : m_lines.back().yOffset) - 1.f;
        m_lines.push_back({text /* TODO */, std::move(texture), startY, addTime - startY + (float)countLines * timePerLine, (unsigned)countLines, color});
    }
}

void Subtitles::draw(const DrawTarget &target, float timeAlive) {
    if(m_lines.empty()) {
        m_lastT = timeAlive;
        return;
    }
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    Coords pixelSize = m_instance.graphics().displayTarget().pixelSize();
    float lowest = m_lines.empty() ? 0.f : std::min(m_lines.back().yOffset, 0.f);
    float dy = std::min((timeAlive - m_lastT) * speed, -lowest);
    LOGD("%f", dy);
    m_lastT = timeAlive;
    for(auto& line : m_lines)
        line.yOffset += dy;
    while(!m_lines.empty() && (m_lines.front().yOffset > 5 || m_lines.front().hideTime < timeAlive))
        m_lines.erase(m_lines.begin(), m_lines.begin() + (int)m_lines.front().groupSize);
    for(const auto& line : m_lines)
        if(line.yOffset >= -1.f) {
            float destY = pixelSize.fy() - (float)line.texture.height() * (1.f + line.yOffset);
            target.blit(line.texture, copyProgram, 0, (int)destY, 0, 0, DrawTarget::fullSize,
                        DrawTarget::fullSize, pixelSize.x(), pixelSize.y());
        }
}

void Subtitles::refresh() {
    for(auto& line : m_lines) {
        if(!line.texture.live())
            line.texture = m_instance.graphics().renderLine(line.text);
    }
}