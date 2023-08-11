#include "leveltitle.h"

LevelTitle::LevelTitle(Instance& instance) :
    m_instance(instance),
    m_font(decoders::ttf(instance, LevelTitle::fontFilename))
{ }

void LevelTitle::show(const std::string& text) {
    m_image.emplace(TextImage::create(m_instance, *m_font, text));
    m_hide.reset();
}

void LevelTitle::hide() {
    m_image.reset();
    m_hide.reset();
}

void LevelTitle::fadeout() {
    m_hide = std::chrono::steady_clock::now() + lingerTime + fadeoutTime;
}

void LevelTitle::update() {
    if(m_hide) {
        std::chrono::duration<float> timeLeft = m_hide.value() - std::chrono::steady_clock::now();
        m_opacity = std::clamp(timeLeft / fadeoutTime, 0.f, 1.f);
    } else
        m_opacity = 1.f;
    if(m_opacity == 0.f) {
        hide();
        return;
    }
}

void LevelTitle::draw(DrawTarget& target) {
    if(!m_image)
        return;

    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    constexpr FCoords offset{Graphics::baseDim.x / 2.f - maxWidth / 2.f, startY};
    constexpr FCoords rect{maxWidth, endY - startY};

    auto program = m_instance.graphics().shaders().titleText({ .color = colorBg.gl(m_opacity) });
    target.draw(m_image.value(), program, coords, {
        .dest = offset + shadow,
        .area = rect
    });

    program.params().color = colorFg.gl(m_opacity);
    target.draw(m_image.value(), program, coords, {
        .dest = offset,
        .area = rect
    });
}

void LevelTitle::resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    m_font->setSizes(fontSize, 0.f, coords.scale);
    /*if(m_image)
        m_image->render(); TODO */
}
