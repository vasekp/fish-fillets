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

void Subtitles::defineColor(const std::string &name, Color color) {
    m_colors.insert({name, color});
}

void Subtitles::add(const std::string &text, const std::string& colorName, float addTime) {
    auto lines = breakLines(text);
    auto countLines = lines.size();
    for(auto& line : lines) {
        float displayTime = std::max((float)text.length() * timePerChar, minTimePerLine);
        Color color = [&]() {
            if(m_colors.contains(colorName))
                return m_colors.at(colorName);
            else {
                LOGE("Unknown color: %s", colorName.c_str());
                return Color{255, 255, 255};
            }
        }();
        m_lines.push_back({line, m_instance.graphics().renderLine(line), false, 0.f, displayTime,
                           (unsigned) countLines, color});
    }
}

void Subtitles::draw(const DrawTarget &target, float dTime, float absTime) {
    if(m_lines.empty())
        return;
    const auto& textProgram = m_instance.graphics().shaders().wavyText;
    Coords pixelSize = m_instance.graphics().displayTarget().pixelSize();
    auto liveEnd = std::find_if(m_lines.begin(), m_lines.end(), [](const auto& line) { return !line.live; });
    float lowest = std::accumulate(m_lines.begin(), liveEnd, 0.f, [](float y, const auto& line) { return std::min(y, line.yOffset); });
    float dy = std::min(dTime * speed, -lowest);
    for(auto& line : m_lines)
        line.yOffset += dy;
    if(lowest == 0.f && liveEnd != m_lines.end()) {
        auto& line = *liveEnd;
        line.live = true;
        line.yOffset = -1.f;
        line.hideTime += absTime;
    }
    while(!m_lines.empty()) {
        const auto &front = m_lines.front();
        if (front.live && (front.yOffset > 5 || front.hideTime < absTime))
            m_lines.erase(m_lines.begin(), m_lines.begin() + (int) m_lines.front().groupSize);
        else
            break;
    }
    for(const auto& line : m_lines)
        if(line.live) {
            glUseProgram(textProgram);
            glUniform4fv(textProgram.uniform("uColor"), 1, line.color.gl().get());
            float destY = pixelSize.fy() - (float)line.texture.height() * (1.f + line.yOffset);
            target.blit(line.texture, textProgram, 0, (int)destY, 0, 0, DrawTarget::fullSize,
                        DrawTarget::fullSize, pixelSize.x(), pixelSize.y());
        }
}

void Subtitles::refresh() {
    for(auto& line : m_lines) {
        if(!line.texture.live())
            line.texture = m_instance.graphics().renderLine(line.text);
    }
}
