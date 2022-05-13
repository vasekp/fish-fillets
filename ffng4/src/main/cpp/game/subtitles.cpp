#include "subtitles.h"

std::vector<std::string> Subtitles::breakLines(const std::string &text) {
    auto &jni = m_instance.jni();
    auto jFilename = jni->NewStringUTF("font/font_subtitle.ttf");
    auto jText = jni->NewStringUTF(text.c_str());
    auto jArray = (jobjectArray) jni->CallObjectMethod(jni.object(), jni.method("breakLines"),
                                                       jText, jFilename, 16.f,
                                                       m_instance.graphics().displayTarget().pixelSize().x());
    auto length = jni->GetArrayLength(jArray);
    std::vector<std::string> ret{};
    ret.reserve(length);
    for (auto i = 0u; i < length; i++) {
        auto jLine = (jstring)jni->GetObjectArrayElement(jArray, (int) i);
        auto chars = jni->GetStringUTFChars(jLine, nullptr);
        ret.emplace_back(chars);
        jni->ReleaseStringUTFChars(jLine, chars);
        jni->DeleteLocalRef(jLine);
    }
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    jni->DeleteLocalRef(jArray);
    return ret;
}

void Subtitles::add(const std::string &text, Color color, float addTime) {
    auto lines = breakLines(text);
    auto countLines = lines.size();
    for(auto& line : lines) {
        float startY = (m_lines.empty() ? 0.f : m_lines.back().yOffset) - 1.f;
        m_lines.push_back({line, m_instance.graphics().renderLine(line), startY,
                           addTime - startY + (float) countLines * timePerLine,
                           (unsigned) countLines, color});
    }
}

void Subtitles::draw(const DrawTarget &target, float dTime, float absTime) {
    if(m_lines.empty())
        return;
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    Coords pixelSize = m_instance.graphics().displayTarget().pixelSize();
    float lowest = m_lines.empty() ? 0.f : std::min(m_lines.back().yOffset, 0.f);
    float dy = std::min(dTime * speed, -lowest);
    for(auto& line : m_lines)
        line.yOffset += dy;
    while(!m_lines.empty() && (m_lines.front().yOffset > 5 || absTime > m_lines.front().hideTime))
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
