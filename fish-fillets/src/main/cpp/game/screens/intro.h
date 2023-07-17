#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "gamescreen.h"
#include "subsystem/audio.h"
#include "api/ogg/video.h"
#include "baseinput.h"

class IntroScreen : public GameScreen {
    BaseInput m_input;

    ogg::InterleavedStream m_ogg;
    ogg::VorbisDecoder m_vorbis;
    ogg::TheoraDecoder m_theora;

    AudioSourceQueue::Ref m_aBuffer;
    std::deque<ogg::TheoraDecoder::Frame> m_vBuffer;

    Texture m_texY;
    Texture m_texCb;
    Texture m_texCr;
    float m_texTime;

public:
    IntroScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_update() override;
    void own_draw(DrawTarget& target) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords) override;

private:
    void fill_buffers();

    static constexpr std::size_t vBufSize = 5;
};

#endif //FISH_FILLETS_GAME_INTRO_H
