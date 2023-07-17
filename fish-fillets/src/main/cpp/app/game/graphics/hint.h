#ifndef FISH_FILLETS_GAME_HINT_H
#define FISH_FILLETS_GAME_HINT_H

#include "subsystem/graphics.h"

class Hint {
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::vector<TextImage> m_lines;

public:
    Hint(Instance& instance, const std::string& text);

    void draw(DrawTarget& target);
    void resize();

    constexpr static const char* fontFilename = "font/font_subtitle.ttf";
    constexpr static float fontSize = 20.f; // in "reduced base pixels": 640x480 window after reducing button strip
    constexpr static float outline = 2.f;
};

#endif //FISH_FILLETS_GAME_HINT_H
