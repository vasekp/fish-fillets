#ifndef FISH_FILLETS_CREDITS_H
#define FISH_FILLETS_CREDITS_H

#include "screen.h"

class CreditsScreen : public GameScreen {
    unsigned m_viewWidth;
    unsigned m_viewHeight;
    unsigned m_totalHeight;

public:
    CreditsScreen(Instance &);

private:
    void own_load() override;
    void own_draw() override;
    bool own_mouse(unsigned, unsigned) override;
};

#endif //FISH_FILLETS_CREDITS_H