#ifndef FISH_FILLETS_CREDITS_H
#define FISH_FILLETS_CREDITS_H

#include "gamescreen.h"
#include "subsystem/audio.h"
#include "baseinput.h"

class CreditsScreen : public GameScreen {
public:
    CreditsScreen(Instance&);
    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_draw(DrawTarget& target) override;
    bool own_pointer(FCoords coords) override;
    bool own_key(Key key) override;

private:
    BaseInput m_input;
    AudioSource::Ref m_music;
    float m_totalHeight;

    void quit();
};

#endif //FISH_FILLETS_CREDITS_H
