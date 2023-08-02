#include "subsystem/graphics.h"

void IFont::setSizes(float fontSize, float outline, float scale) {
    fontSize *= scale;
    outline *= scale;
    if(fontSize == m_fontSize && outline == m_outline && scale == m_scale)
        return;
    Log::debug<Log::graphics>("fontSize ", fontSize, " outline ", outline, " scale ", scale);
    m_fontSize = fontSize;
    m_outline = outline;
    m_scale = scale;
    own_setSizes();
}

std::vector<std::string> IFont::breakLines(const std::string& text, float width) {
    return own_breakLines(text, width * m_scale);
}

Texture IFont::renderText(Instance& instance, const std::string& text) const {
    auto [width, height, data] = own_renderText(text);
    ICoords physSize = {(int)width, (int)height};
    FCoords logSize = FCoords{physSize} / m_scale;
    return Texture{instance.graphics().system(), data.get(), logSize, physSize};
}
