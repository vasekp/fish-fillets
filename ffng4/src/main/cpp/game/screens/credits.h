#ifndef FISH_FILLETS_CREDITS_H
#define FISH_FILLETS_CREDITS_H

#include "screen.h"

class CreditsScreen : public GameScreen {
    BaseInput m_input;
    std::shared_ptr<AudioSource> m_music;
    unsigned m_viewWidth;
    unsigned m_viewHeight;
    unsigned m_totalHeight;

public:
    CreditsScreen(Instance &);
    IInput &input() override { return m_input; }

protected:
    void own_start() override;
    void own_refresh() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_pointer(FCoords coords, bool longPress) override;
    bool own_key(Key key) override;

private:
    void quit();
};

#endif //FISH_FILLETS_CREDITS_H