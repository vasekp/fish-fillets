#ifndef FISH_FILLETS_GAME_WORLDMAP_H
#define FISH_FILLETS_GAME_WORLDMAP_H

#include "screen.h"
#include "game/structure/gametree.h"
#include "subsystem/audio.h"
#include "pedometer.h"

class WorldMap : public GameScreen {
    enum class Frames {
        none,
        loading,
        exit,
        options,
        intro,
        credits
    };

    BaseInput m_input;
    AudioSource::Ref m_music;
    std::vector<const Image*> m_nodeImages;
    std::map<Frames, Color> m_maskColors;
    std::vector<const LevelRecord*> m_open;
    std::vector<const LevelRecord*> m_forks;
    Frames m_staticFrame;
    std::optional<Pedometer> m_pm;

    static constexpr int nodeRadius = 9;
    static constexpr int nodeTolerance = 15;

    struct MaskColors {
        static constexpr Color exit = 0x008080;
        static constexpr Color options = 0x008000;
        static constexpr Color intro = 0x000080;
        static constexpr Color credits = 0x808000;
        static constexpr Color mainBranch = 0xFFFFFF;
    };

    struct ActiveAreas {
        FCoords from;
        FCoords to;
        Frames frame;
    };

    static constexpr std::array<ActiveAreas, 5> areas {{
            {{0, 0}, {138, 127}, Frames::intro},
            {{517, 0}, {640, 130}, Frames::exit},
            {{0, 364}, {124, 480}, Frames::credits},
            {{0, 403}, {145, 480}, Frames::credits}, /* a single rectangle would overlap a level */
            {{487, 362}, {640, 480}, Frames::options}
    }};

public:
    WorldMap(Instance&);
    void staticFrame(Frames frame);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_resume() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords, bool longPress) override;

private:
    void drawMasked(const DrawTarget& target, Color maskColor);
};

#endif //FISH_FILLETS_GAME_WORLDMAP_H
