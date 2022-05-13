#ifndef FFNG_GAME_SUBTITLES_H
#define FFNG_GAME_SUBTITLES_H

#include "subsystem/graphics.h"

class Subtitles {
    struct Title {
        std::string text;
        ogl::Texture texture;
        float yOffset;
        float hideTime;
        unsigned groupSize;
        Color color;
    };

    Instance& m_instance;
    std::deque<Title> m_lines;
    float m_lastT;

public:
    Subtitles(Instance& instance) : m_instance(instance), m_lastT(0) { }

    void add(const std::string& text, Color color, float addTime);
    void draw(const DrawTarget& target, float timeAlive);
    void refresh();

    constexpr static const char* filename = "font/font_subtitle.ttf";
    constexpr static float fontsize = 16.f;
    constexpr static float outline = 2.f;
    constexpr static float timePerLine = 2.f;
    constexpr static float speed = 1.f;
};

#endif //FFNG_GAME_SUBTITLES_H