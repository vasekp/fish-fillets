#include "subtitles.h"

std::vector <std::string> Subtitles::breakLines(const std::string& text) {
    /*auto& jni = m_instance.jni(); // XXX
    auto jFilename = jni->NewStringUTF("font/font_subtitle.ttf");
    auto jText = jni->NewStringUTF(text.c_str());
    auto jArray = (jobjectArray) jni->CallObjectMethod(jni.object(), jni.method("breakLines"),
            jText, jFilename, fontsize * m_instance.graphics().dpi(),
            m_instance.graphics().fullscreenTarget().reducedDisplaySize().x());
    auto length = jni->GetArrayLength(jArray);
    std::vector<std::string> ret{};
    ret.reserve(length);
    for(auto i = 0u ; i < length ; i++) {
        auto jLine = (jstring) jni->GetObjectArrayElement(jArray, (int) i);
        auto chars = jni->GetStringUTFChars(jLine, nullptr);
        ret.emplace_back(chars);
        jni->ReleaseStringUTFChars(jLine, chars);
        jni->DeleteLocalRef(jLine);
    }
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    jni->DeleteLocalRef(jArray);
    return ret;*/
    return {};
}

void Subtitles::defineColors(const std::string& name, Color color1, Color color2) {
    m_colors.insert({name, {color1, color2}});
}

void Subtitles::add(const std::string& text, const std::string& colors) {
    auto lines = breakLines(text);
    auto countLines = lines.size();
    for(const auto& line : lines) {
        float duration = std::max((float)text.length() * timePerChar, minTimePerLine);
        auto [color1, color2] = [&]() {
            if(m_colors.contains(colors))
                return m_colors.at(colors);
            else {
                Log::warn("Unknown color: ", colors);
                return std::pair{Color::white, Color::white};
            }
        }();
        auto texture = m_instance.graphics().renderText(line, "font/font_subtitle.ttf", fontsize * m_instance.graphics().dpi(), outline * m_instance.graphics().dpi());
        m_lines.push_back({
                line, std::move(texture),
                false, 0.f, 0.f, duration,
                (unsigned)countLines, color1, color2
        });
    }
}

void Subtitles::clear() {
    m_lines.clear();
}

void Subtitles::draw(const DrawTarget& target, float dTime, float absTime) {
    if(m_lines.empty())
        return;
    const auto& textProgram = m_instance.graphics().shaders().wavyText;
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
    }
    while(!m_lines.empty()) {
        const auto& front = m_lines.front();
        if(front.live && (front.yOffset > 5 || absTime - front.addTime > front.duration))
            m_lines.erase(m_lines.begin(), m_lines.begin() + (int)m_lines.front().groupSize);
        else
            break;
    }
    for(const auto& line : m_lines)
        if(line.live) {
            glUseProgram(textProgram);
            glUniform4fv(textProgram.uniform("uColor1"), 1, line.color1.gl().data());
            glUniform4fv(textProgram.uniform("uColor2"), 1, line.color2.gl().data());
            glUniform1f(textProgram.uniform("uTime"), absTime - line.addTime);
            auto width = line.texture.width();
            auto height = line.texture.height() - outline * m_instance.graphics().dpi();
            const auto& fullscreen = dynamic_cast<const DisplayTarget&>(target);
            float destX = fullscreen.displayOffset().fx() + (fullscreen.reducedDisplaySize().fx() - (float)width) / 2.f;
            float destY = (float)m_instance.graphics().display().height() - (float)height * (1.5f + line.yOffset);
            target.blit(line.texture, textProgram, destX, destY - (float)height, 0, 0, DrawTarget::fullSize, 3 * height);
        }
}

void Subtitles::refresh() {
    for(auto& line : m_lines) {
        if(!line.texture.live())
            line.texture = m_instance.graphics().renderText(line.text, "font/font_subtitle.ttf", fontsize * m_instance.graphics().dpi(), outline * m_instance.graphics().dpi());
    }
}
