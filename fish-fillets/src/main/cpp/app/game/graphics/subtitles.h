#ifndef FISH_FILLETS_GAME_SUBTITLES_H
#define FISH_FILLETS_GAME_SUBTITLES_H

#include "common.h"
#include "instance.h"
#include "subsystem/graphics/image.h"
#include "game/screens/liveclock.h"

class DrawTarget;

class Subtitles {
public:
    Subtitles(Instance& instance);

    void add(const std::string& text, const std::string& colors);
    void defineColors(const std::string& name, Color color1, Color color2);
    void update(LiveClock::time_point time);
    void draw(DrawTarget& target, LiveClock::time_point time);
    void resize();
    void clear();

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
