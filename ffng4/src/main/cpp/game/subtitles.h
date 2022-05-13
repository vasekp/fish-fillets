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
        Color color1;
        Color color2;
    };

    Instance& m_instance;
    std::deque<Title> m_lines;
    std::map<std::string, std::pair<Color, Color>> m_colors;

public:
    Subtitles(Instance& instance) : m_instance(instance) { }

    void add(const std::string& text, const std::string& colors, float addTime);
    void defineColors(const std::string& name, Color color1, Color color2);
    void draw(const DrawTarget& target, float dTime, float absTime);
    void refresh();

    constexpr static const char* filename = "font/font_subtitle.ttf";
    constexpr static float fontsize = 16.f;
    constexpr static float outline = 3.f;
    constexpr static float timePerChar = 0.09f;
    constexpr static float minTimePerLine = 2.5f;
    constexpr static float speed = 1.f;

private:
    std::vector<std::string> breakLines(const std::string& text);
};

#endif //FFNG_GAME_SUBTITLES_H