#ifndef FFNG_GAME_SUBTITLES_H
#define FFNG_GAME_SUBTITLES_H

#include "subsystem/graphics.h"

class Subtitles {
    struct Title {
        TextImage image;
        bool live;
        float yOffset;
        float addTime;
        float duration;
        unsigned groupSize;
        Color color1;
        Color color2;
    };

    Instance& m_instance;
    Font m_font;
    std::deque<Title> m_lines;
    std::map<std::string, std::pair<Color, Color>> m_colors;

public:
    Subtitles(Instance& instance);

    void add(const std::string& text, const std::string& colors);
    void defineColors(const std::string& name, Color color1, Color color2);
    void draw(const DrawTarget& target, float dTime, float absTime);
    void refresh();
    void clear();

    constexpr static const char* fontFilename = "font/font_subtitle.ttf";
    constexpr static float fontsize = 0.1f;
    constexpr static float outline = 0.01f; // both relative to DPI
    constexpr static float timePerChar = 0.09f;
    constexpr static float minTimePerLine = 2.5f;
    constexpr static float speed = 1.f;
};

#endif //FFNG_GAME_SUBTITLES_H
