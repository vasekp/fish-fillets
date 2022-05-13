#ifndef FFNG_LEVEL_TITLE_H
#define FFNG_LEVEL_TITLE_H

#include "subsystem/graphics.h"

class LevelTitle {
    Instance& m_instance;
    ogl::Texture m_texture;

public:
    LevelTitle(Instance& instance) : m_instance(instance) { }

    void refresh();
    void draw();

private:
    constexpr static float startY = 430.f / 480.f;
    constexpr static float endY = 470.f / 480.f;
    constexpr static Color colorFg{255, 210, 40};
    constexpr static Color colorBg{40, 20, 6};
};

#endif //FFNG_LEVEL_TITLE_H