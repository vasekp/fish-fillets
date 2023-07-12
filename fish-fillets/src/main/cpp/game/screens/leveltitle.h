#ifndef FISH_FILLETS_LEVEL_TITLE_H
#define FISH_FILLETS_LEVEL_TITLE_H

#include "subsystem/graphics.h"

class LevelTitle {
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::optional<TextImage> m_image;
    std::optional<std::chrono::steady_clock::time_point> m_hide;
    float m_opacity;

public:
    LevelTitle(Instance& instance);

    void show(const std::string& text);
    void fadeout();
    void hide();
    void resize();
    void update();
    void draw(const DrawTarget& target);

private:
    constexpr static const char* fontFilename = "font/font_title.ttf";
    constexpr static float fontSize = 18.f; // base pixels
    constexpr static float startY = 400.f;
    constexpr static float endY = 470.f;
    constexpr static float maxWidth = 480.f;
    constexpr static Color colorFg{255, 210, 40};
    constexpr static Color colorBg{40, 20, 6};
    constexpr static FCoords shadow{2.f, 2.f};
    constexpr static std::chrono::steady_clock::duration lingerTime = 1200ms;
    constexpr static std::chrono::steady_clock::duration fadeoutTime = 300ms;
};

#endif //FISH_FILLETS_LEVEL_TITLE_H
