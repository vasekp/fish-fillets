#ifndef FISH_FILLETS_LEVEL_TITLE_H
#define FISH_FILLETS_LEVEL_TITLE_H

#include "common.h"
#include "instance.h"
#include "subsystem/graphics/image.h"

class DrawTarget;
class IFont;

class LevelTitle {
public:
    LevelTitle(Instance& instance);
    ~LevelTitle();

    void show(const std::string& text);
    void fadeout();
    void hide();
    void resize();
    void update();
    void draw(DrawTarget& target);

private:
    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::optional<ImageRef> m_image;
    std::optional<std::chrono::steady_clock::time_point> m_hide;
    float m_opacity;
};

#endif //FISH_FILLETS_LEVEL_TITLE_H
