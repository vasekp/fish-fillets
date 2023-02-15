#ifndef FFNG_LEVEL_TITLE_H
#define FFNG_LEVEL_TITLE_H

#include "subsystem/graphics.h"

class LevelTitle {
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;

    std::optional<TextImage> m_image;

public:
    LevelTitle(Instance& instance);

    void set(const std::string& text);
    void reset();
    void refresh();
    void draw(const DrawTarget& target, float opacity);

private:
    constexpr static const char* fontFilename = "font/font_title.ttf";
    constexpr static float fontSize = 18.f; // base pixels
    constexpr static float startY = 400.f;
    constexpr static float endY = 470.f;
    constexpr static float maxWidth = 480.f;
    constexpr static Color colorFg{255, 210, 40};
    constexpr static Color colorBg{40, 20, 6};
    constexpr static FCoords shadow{2.f, 2.f};
};

#endif //FFNG_LEVEL_TITLE_H
