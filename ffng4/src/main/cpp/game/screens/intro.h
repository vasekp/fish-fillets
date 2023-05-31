#ifndef FISH_FILLETS_GAME_INTRO_H
#define FISH_FILLETS_GAME_INTRO_H

#include "screen.h"
#include "subsystem/audio.h"
#include "theora-helpers.h"
#include <forward_list>

using namespace std::string_view_literals; // TODO global

class IntroAudioSource : public AudioSourceBase {
    std::forward_list<std::vector<float>> m_queue;
    decltype(m_queue)::iterator m_curVector;
    std::atomic<decltype(m_queue)::iterator> m_end;
    decltype(m_queue)::iterator m_last;
    std::size_t m_curIndex;
    std::size_t m_total;
    std::size_t m_played = 0;

    std::chrono::steady_clock::time_point m_relStartTime;

public:
    IntroAudioSource() : m_queue{}, m_curVector{m_queue.end()}, m_end{m_queue.end()}, m_last{m_queue.before_begin()}, m_total{0} { }

    using Ref = std::shared_ptr<IntroAudioSource>;

    void push_back(std::vector<float>&& vector) {
        m_total += vector.size();
        m_queue.insert_after(m_last, std::move(vector));
        m_last++;
        m_end = m_queue.end();
    }

    void start() {
        m_curVector = m_queue.begin();
        m_curIndex = 0;
        m_relStartTime = std::chrono::steady_clock::now();
    }

    /* Side-effect: frees no longer needed vectors, leaves empty queue at end */
    void mixin(float output[], std::size_t numSamples) override {
        Log::debug("mixin ", numSamples, " @ ", std::chrono::duration<float>(std::chrono::steady_clock::now() - m_relStartTime).count(), " total ", m_played);
        for(auto i = 0u; i < numSamples; i++) {
            if((i + m_played) % 22050 > 20580) {
                output[i] += 0.15 * (i & 2);
                if((i + m_played) % 88200 < 22050)
                    output[i] += 0.15 * (i & 2);
            }
        }
        if(m_curIndex + numSamples < m_curVector->size()) {
            for(auto i = 0u; i < numSamples; i++)
                output[i] += m_volume * (*m_curVector)[m_curIndex++];
            m_played += numSamples;
        } else if(!done()) {
            auto countRead = m_curVector->size() - m_curIndex;
            for(auto i = 0u; i < countRead; i++)
                output[i] += m_volume * (*m_curVector)[m_curIndex++];
            if(std::next(m_curVector) != m_end.load()) {
                m_curVector++;
                m_queue.pop_front();
            } else {
                Log::error("Audio data underflow!");
            }
            m_curIndex = 0;
            m_played += countRead;
            return mixin(output + countRead, numSamples - countRead);
        }
    }

    bool done() const override { return m_curVector == m_end.load(); } // called from audio thread only

    std::size_t total() const { return m_total; }

    std::string_view name() const override { return "intro audio"sv; }
};

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

    constexpr static int buffSize = 2;
    struct Frame {
        std::array<unsigned char, 640 * 480> data_y;
        std::array<unsigned char, 320 * 240> data_cb;
        std::array<unsigned char, 320 * 240> data_cr;
        float time;
    };
    std::deque<Frame> m_vBuffer;

    IntroAudioSource::Ref m_aBuffer;

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
};

#endif //FISH_FILLETS_GAME_INTRO_H
