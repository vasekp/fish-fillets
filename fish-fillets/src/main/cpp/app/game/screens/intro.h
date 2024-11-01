#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "common.h"
#include "instance.h"
#include "gamescreen.h"
#include "subsystem/audio/audiosource.h"
#include "api/ogg/video.h"
#include "baseinput.h"

class IntroScreen : public GameScreen {
public:
    IntroScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_resume() override;
    void own_update() override;
    void own_draw(DrawTarget& target) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords) override;

private:
    BaseInput m_input;

    ogg::DemuxStream m_demux;
    ogg::VorbisDecoder m_vorbis;
    ogg::TheoraDecoder m_theora;

    AudioSourceQueue::Ref m_aBuffer;
    std::deque<ogg::TheoraDecoder::Frame> m_vBuffer;

    ImageRef m_imgY;
    ImageRef m_imgCb;
    ImageRef m_imgCr;
    float m_imgTime;

    void fill_buffers();
};

#endif //FISH_FILLETS_GAME_INTRO_H
