#ifndef FISH_FILLETS_GAME_POSTER_H
#define FISH_FILLETS_GAME_POSTER_H

#include "common.h"
#include "instance.h"
#include "gamescreen.h"
#include "game/graphics/hint.h"
#include "subsystem/script.h"
#include "subsystem/audio/audiosource.h"
#include "baseinput.h"

class PosterScreen : public GameScreen, public ScriptReferrer {
public:
    PosterScreen(Instance& instance, const std::string& script);

protected:
    IInputSink& input() override { return m_input; }

    void own_start() override;
    void own_draw(DrawTarget& target) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords) override;
    void own_resize() override;

private:
    BaseInput m_input;
    AudioSource::Ref m_music;
    Script m_script;
    std::optional<Hint> m_hint;

    void quit();

    void poster_display(const std::string& filename);
    void poster_translation(const std::string& text);

    constexpr static auto minTime = 3s;
};

#endif //FISH_FILLETS_GAME_POSTER_H
