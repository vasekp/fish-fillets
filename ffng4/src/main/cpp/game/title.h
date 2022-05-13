#ifndef FFNG_LEVEL_TITLE_H
#define FFNG_LEVEL_TITLE_H

#include "subsystem/graphics.h"

class LevelTitle {
    Instance& m_instance;
    std::string m_text;
    ogl::Texture m_texture;

public:
    LevelTitle(Instance& instance, std::string text) : m_instance(instance), m_text(std::move(text)) { }

    void refresh();
    void draw(const DrawTarget& target, float opacity);

private:
    constexpr static float startY = 400.f / 480.f;
    constexpr static float endY = 470.f / 480.f;
    constexpr static float maxWidth = 480.f / 480.f;
    constexpr static Color colorFg{255, 210, 40};
    constexpr static Color colorBg{40, 20, 6};
};

#endif //FFNG_LEVEL_TITLE_H