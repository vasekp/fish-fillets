#ifndef FISH_FILLETS_GAME_HINT_H
#define FISH_FILLETS_GAME_HINT_H

#include "common.h"
#include "instance.h"
#include "subsystem/graphics.h"

class Hint {
public:
    Hint(Instance& instance, const std::string& text, bool fullWidth);

    void draw(DrawTarget& target);
    void resize();

    constexpr static const char* fontFilename = "font/font_subtitle.ttf";
    constexpr static float fontSize = 20.f; // in "reduced base pixels": 640x480 window after reducing button strip
    constexpr static float outline = 2.f;

private:
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::vector<ImageRef> m_lines;
    Graphics::CoordSystems m_coords;
};

#endif //FISH_FILLETS_GAME_HINT_H
