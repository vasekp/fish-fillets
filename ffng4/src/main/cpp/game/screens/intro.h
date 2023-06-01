#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "screen.h"
#include "subsystem/audio.h"
#include "api/ogg.h"

using namespace std::string_view_literals; // TODO global

class IntroScreen : public GameScreen {
    BaseInput m_input;

    ogg::OggSync m_ogg;
    ogg::TheoraInfo m_thInfo;
    ogg::TheoraComment m_thComment;
    ogg::TheoraSetup m_thSetup;
    ogg::TheoraDecoder m_thDecoder;
    ogg::VorbisInfo m_vbInfo;
    ogg::VorbisComment m_vbComment;
    std::unique_ptr<ogg::OggStream> m_thStream;
    std::unique_ptr<ogg::OggStream> m_vbStream;
    std::unique_ptr<ogg::VorbisDecoder> m_vbDecoder;

    struct Frame {
        std::array<unsigned char, 640 * 480> data_y;
        std::array<unsigned char, 320 * 240> data_cb;
        std::array<unsigned char, 320 * 240> data_cr;
        float time;
    };
    std::deque<Frame> m_vBuffer;

    AudioSourceQueue::Ref m_aBuffer;

public:
    IntroScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords, bool longPress) override;

private:
    void queue_page(ogg_page&);
    void fill_buffers();

    static constexpr std::size_t vBufSize = 5;
};

#endif //FISH_FILLETS_GAME_INTRO_H
