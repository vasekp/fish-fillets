#include "subsystem/graphics.h"
#include "subsystem/graphics/texture.h"

constexpr float upsampleThreshold = 20.f;

void IFont::setSizes(float fontSize, float outline, float scale) {
    fontSize *= scale;
    outline *= scale;
    if(fontSize < upsampleThreshold) {
        fontSize *= 2;
        outline *= 2;
        scale *= 2;
    }
    if(fontSize == m_fontSize && outline == m_outline && scale == m_scale)
        return;
    Log::debug<Log::graphics>("fontSize ", fontSize, " outline ", outline, " scale ", scale);
    m_fontSize = fontSize;
    m_outline = outline;
    m_scale = scale;
    own_setSizes();

    for(auto* image : m_images)
        image->render(m_instance);
}

std::vector<std::string> IFont::breakLines(const std::string& text, float width) {
    return own_breakLines(text, width * m_scale);
}

std::unique_ptr<Texture> IFont::renderText(Instance& instance, const std::string& text) const {
    auto [physSize, data] = own_renderText(text);
    FCoords logSize = FCoords{physSize} / m_scale;
    return std::make_unique<Texture>(instance.graphics().system(), TextureType::image, physSize, logSize, data.get());
}

void IFont::regImage(TextImage* image) {
    m_images.insert(image);
}

void IFont::unregImage(TextImage* image) {
    m_images.erase(image);
}
