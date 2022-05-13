#ifndef FFNG_GAME_SUBTITLES_H
#define FFNG_GAME_SUBTITLES_H

#include "subsystem/graphics.h"

class Subtitles {
    struct Title {
        std::string text;
        ogl::Texture texture;
        bool live;
        float yOffset;
        float hideTime;
        unsigned groupSize;
        Color color;
    };

    Instance& m_instance;
    std::deque<Title> m_lines;

public:
    Subtitles(Instance& instance) : m_instance(instance) { }

    void add(const std::string& text, Color color, float addTime);
    void draw(const DrawTarget& target, float dTime, float absTime);
    void refresh();

    constexpr static const char* filename = "font/font_subtitle.ttf";
    constexpr static float fontsize = 16.f;
    constexpr static float outline = 2.f;
    constexpr static float timePerChar = 0.09f;
    constexpr static float minTimePerLine = 2.f;
    constexpr static float speed = 1.f;

private:
    std::vector<std::string> breakLines(const std::string& text);
};

#endif //FFNG_GAME_SUBTITLES_H