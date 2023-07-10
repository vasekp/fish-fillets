#ifndef FISH_FILLETS_CREDITS_H
#define FISH_FILLETS_CREDITS_H

#include "screen.h"
#include "subsystem/audio.h"

class CreditsScreen : public GameScreen {
    BaseInput m_input;
    AudioSource::Ref m_music;
    unsigned m_totalHeight;

public:
    CreditsScreen(Instance &);
    IInputSink &input() override { return m_input; }

protected:
    void own_start() override;
    void own_draw(const DrawTarget& target) override;
    bool own_pointer(FCoords coords) override;
    bool own_key(Key key) override;

private:
    void quit();
};

#endif //FISH_FILLETS_CREDITS_H
