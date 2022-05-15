#include "subtitles.h"

std::vector<std::string> Subtitles::breakLines(const std::string &text) {
    auto &jni = m_instance.jni();
    auto jFilename = jni->NewStringUTF("font/font_subtitle.ttf");
    auto jText = jni->NewStringUTF(text.c_str());
    auto jArray = (jobjectArray) jni->CallObjectMethod(jni.object(), jni.method("breakLines"),
                                                       jText, jFilename, 16.f,
                                                       m_instance.graphics().windowTarget().pixelSize().x());
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

void Subtitles::defineColors(const std::string &name, Color color1, Color color2) {
    m_colors.insert({name, {color1, color2}});
}

void Subtitles::add(const std::string &text, const std::string& colors, float addTime) {
    auto lines = breakLines(text);
    auto countLines = lines.size();
    for(auto& line : lines) {
        float duration = std::max((float) text.length() * timePerChar, minTimePerLine);
        auto[color1, color2] = [&]() {
            if (m_colors.contains(colors))
                return m_colors.at(colors);
            else {
                LOGE("Unknown color: %s", colors.c_str());
                return std::pair{Color::white, Color::white};
            }
        }();
        auto texture = m_instance.graphics().renderText(line, "font/font_subtitle.ttf", 16.f, 2.f,
                                                        m_instance.graphics().windowTarget());
        m_lines.push_back(
                {line, std::move(texture),
                 false, 0.f, 0.f, duration,
                 (unsigned) countLines, color1, color2});
    }
}

void Subtitles::draw(const DrawTarget &target, float dTime, float absTime) {
    if(m_lines.empty())
        return;
    const auto& textProgram = m_instance.graphics().shaders().wavyText;
    FCoords pixelSize = m_instance.graphics().windowTarget().pixelSize();
    auto liveEnd = std::find_if(m_lines.begin(), m_lines.end(), [](const auto& line) { return !line.live; });
    float lowest = std::accumulate(m_lines.begin(), liveEnd, 0.f, [](float y, const auto& line) { return std::min(y, line.yOffset); });
    float dy = std::min(dTime * speed, -lowest);
    for(auto& line : m_lines)
        line.yOffset += dy;
    if(lowest == 0.f && liveEnd != m_lines.end()) {
        auto& line = *liveEnd;
        line.live = true;
        line.yOffset = -1.f;
        line.addTime = absTime;
    }
    while(!m_lines.empty()) {
        const auto &front = m_lines.front();
        if (front.live && (front.yOffset > 5 || absTime - front.addTime > front.duration))
            m_lines.erase(m_lines.begin(), m_lines.begin() + (int) m_lines.front().groupSize);
        else
            break;
    }
    for(const auto& line : m_lines)
        if(line.live) {
            glUseProgram(textProgram);
            glUniform4fv(textProgram.uniform("uColor1"), 1, line.color1.gl().get());
            glUniform4fv(textProgram.uniform("uColor2"), 1, line.color2.gl().get());
            glUniform1f(textProgram.uniform("uTime"), absTime - line.addTime);
            auto height = line.texture.height();
            float destY = pixelSize.fy() - (float)height * (1.f + line.yOffset);
            target.blit(line.texture, textProgram, 0, destY - (float)height, 0, 0,
                        DrawTarget::fullSize, 3 * height);
        }
}

void Subtitles::refresh() {
    for(auto& line : m_lines) {
        if(!line.texture.live())
            line.texture = m_instance.graphics().renderText(line.text, "font/font_subtitle.ttf", 16.f, 2.f,
                                                            m_instance.graphics().windowTarget());
    }
}