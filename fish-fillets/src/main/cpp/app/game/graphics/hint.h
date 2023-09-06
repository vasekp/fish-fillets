#ifndef FISH_FILLETS_GAME_HINT_H
#define FISH_FILLETS_GAME_HINT_H

#include "common.h"
#include "instance.h"
#include "subsystem/graphics.h"

class Hint {
public:
    Hint(Instance& instance, const std::string& text, bool fullWidth);

    void setAlpha(float alpha);
    void draw(DrawTarget& target);
    void resize();

private:
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::vector<ImageRef> m_lines;
    Graphics::CoordSystems m_coords;
    float m_alpha;
};

#endif //FISH_FILLETS_GAME_HINT_H
