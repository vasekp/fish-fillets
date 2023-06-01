#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "screen.h"
#include "subsystem/audio.h"
#include "theora-helpers.h"

using namespace std::string_view_literals; // TODO global

class IntroScreen : public GameScreen {
    BaseInput m_input;

    OggSync m_oggSync;
    TheoraInfo m_thInfo;
    TheoraComment m_thComment;
    TheoraSetup m_thSetup;
    VorbisInfo m_vbInfo;
    VorbisComment m_vbComment;
    std::unique_ptr<OggStream> m_thStream;
    std::unique_ptr<TheoraDecoder> m_thDecoder;
    std::unique_ptr<OggStream> m_vbStream;
    std::unique_ptr<VorbisDecoder> m_vbDecoder;
    std::unique_ptr<VorbisBlock> m_vbBlock;

    struct Frame {
        std::array<unsigned char, 640 * 480> data_y;
        std::array<unsigned char, 320 * 240> data_cb;
        std::array<unsigned char, 320 * 240> data_cr;
        float time;
    };
    std::deque<Frame> m_vBuffer;

    AudioSourceQueue::Ref m_aBuffer;

    std::string m_data;
    std::size_t m_offset;

public:
    IntroScreen(Instance&);

    IInputSink& input() override { return m_input; }

protected:
    void own_start() override;
    void own_draw(const DrawTarget& target, float dt) override;
    bool own_key(Key key) override;
    bool own_pointer(FCoords coords, bool longPress) override;

private:
    void more_data();
    void queue_page(OggPage&);
    void fill_buffers();

    static constexpr std::size_t vBufSize = 5;
};

#endif //FISH_FILLETS_GAME_INTRO_H
