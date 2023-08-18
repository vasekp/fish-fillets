#ifndef FISH_FILLETS_GAME_SUBTITLES_H
#define FISH_FILLETS_GAME_SUBTITLES_H

#include "subsystem/graphics.h"
#include "game/screens/liveclock.h"

class Subtitles {
public:
    Subtitles(Instance& instance);

    void add(const std::string& text, const std::string& colors);
    void defineColors(const std::string& name, Color color1, Color color2);
    void update(LiveClock::time_point time);
    void draw(DrawTarget& target, LiveClock::time_point time);
    void resize();
    void clear();

    constexpr static const char* fontFilename = "font/font_subtitle.ttf";
    constexpr static float fontSize = 20.f; // in "reduced base pixels": 640x480 window after reducing button strip
    constexpr static float outline = 2.f;
    constexpr static auto timePerChar = 90ms;
    constexpr static auto minTimePerLine = 2500ms;
    constexpr static auto newLineTime = 1s;

private:
    struct Title {
        ImageRef image;
        bool live;
        float yOffset;
        LiveClock::time_point addTime;
        LiveClock::duration duration;
        unsigned groupSize;
        Color color1;
        Color color2;
    };

    Instance& m_instance;
    std::unique_ptr<IFont> m_font;
    std::deque<Title> m_lines;
    std::map<std::string, std::pair<Color, Color>> m_colors;
    LiveClock::time_point m_lastUpdate;
};

#endif //FISH_FILLETS_GAME_SUBTITLES_H
