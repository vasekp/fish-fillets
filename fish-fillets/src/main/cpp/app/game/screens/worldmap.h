#ifndef FISH_FILLETS_GAME_WORLDMAP_H
#define FISH_FILLETS_GAME_WORLDMAP_H

#include "common.h"
#include "instance.h"
#include "gamescreen.h"
#include "subsystem/audio/audiosource.h"
#include "game/structure/levelrecord.h"
#include "pedometer.h"
#include "baseinput.h"

class WorldMap : public GameScreen {
public:
    WorldMap(Instance&);

    enum class Frames {
        none,
        loading,
        help,
        exit,
        options,
        intro,
        credits
    };

protected:
    IInputSink& input() override { return m_input; }

    void own_start() override;
    void own_resume() override;
    void own_update() override;
    void own_draw(DrawTarget& target) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords) override;

private:
    BaseInput m_input;
    AudioSource::Ref m_music;
    std::vector<const Image*> m_nodeImages;
    std::map<Frames, Color> m_maskColors;
    std::vector<const LevelRecord*> m_open;
    std::vector<const LevelRecord*> m_forks;
    Frames m_staticFrame;
    std::function<void()> m_nextAction;
    bool m_frameShown;
    std::optional<Pedometer> m_pm;
    bool m_showEnding;

    void drawMasked(DrawTarget& target, Color maskColor);
    void staticFrame(Frames frame, std::function<void()>&& action);
};

#endif //FISH_FILLETS_GAME_WORLDMAP_H
