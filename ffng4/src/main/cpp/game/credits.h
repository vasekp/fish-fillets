#ifndef FISH_FILLETS_CREDITS_H
#define FISH_FILLETS_CREDITS_H

#include "screen.h"

class CreditsScreen : public GameScreen {
    std::shared_ptr<AudioSource> m_music;
    unsigned m_viewWidth;
    unsigned m_viewHeight;
    unsigned m_totalHeight;

public:
    CreditsScreen(Instance &);

protected:
    void own_start() override;
    void own_refresh() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;
};

#endif //FISH_FILLETS_CREDITS_H