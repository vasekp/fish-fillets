#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "screen.h"
#include "game/structure/gametree.h"
#include "subsystem/audio.h"
#include "pedometer.h"

class IntroScreen : public GameScreen {
    BaseInput m_input;

public:
    IntroScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords, bool longPress) override;
};

#endif //FISH_FILLETS_GAME_INTRO_H
