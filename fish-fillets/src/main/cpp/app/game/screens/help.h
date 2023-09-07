#ifndef FISH_FILLETS_GAME_HELP_H
#define FISH_FILLETS_GAME_HELP_H

#include "common.h"
#include "instance.h"
#include "gamescreen.h"
#include "subsystem/audio/audiosource.h"
#include "api/ogg/video.h"
#include "baseinput.h"
#include "subsystem/graphics/image.h"
#include "game/graphics/hint.h"
#include "liveclock.h"

class HelpScreen : public GameScreen {
public:
    HelpScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_update() override;
    void own_draw(DrawTarget& target) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords) override;
    void own_resize() override;

private:
    BaseInput m_input;

    std::optional<ogg::DemuxStream> m_demux;
    std::optional<ogg::TheoraDecoder> m_theora;
    std::deque<ogg::TheoraDecoder::Frame> m_vBuffer;

    ImageRef m_imgY;
    ImageRef m_imgCb;
    ImageRef m_imgCr;
    float m_imgTime;
    float m_startTime;
    unsigned m_index;

    Hint m_hint;

    struct Button{
        ImageRef image;
        int dir;
        FCoords pos;
        LiveClock::time_point fadeTime;
    };
    std::unique_ptr<IFont> m_buttonFont;
    std::array<Button, 2> m_buttons;

    void loopVideo();
    void loadPart(unsigned i);
    void nextPart();
    void prevPart();
    float timeSinceStart();
    void fill_buffers();
};

#endif //FISH_FILLETS_GAME_HELP_H
