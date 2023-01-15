#ifndef FFNG_LEVEL_TITLE_H
#define FFNG_LEVEL_TITLE_H

#include "subsystem/graphics.h"

class LevelTitle {
    Instance& m_instance;
    TextImage m_image;

public:
    LevelTitle(Instance& instance, IFont& font, std::string text);

    void draw(const DrawTarget& target, float opacity);

    constexpr static const char* fontFilename = "font/font_title.ttf";
    constexpr static float fontSize = 0.1; // relative to DPI

private:
    constexpr static float startY = 400.f / 480.f;
    constexpr static float endY = 470.f / 480.f;
    constexpr static float maxWidth = 480.f / 480.f;
    constexpr static Color colorFg{255, 210, 40};
    constexpr static Color colorBg{40, 20, 6};
};

#endif //FFNG_LEVEL_TITLE_H
