#ifndef FISH_FILLETS_OGG_VIDEO_H
#define FISH_FILLETS_OGG_VIDEO_H

#include "lowlevel.h"

namespace ogg {

    class InterleavedStream;

    class AutoStream : public ll::OggStream {
        InterleavedStream& m_source;

    public:
        AutoStream(InterleavedStream& source, ogg_page&& page);

        bool operator>>(ogg_packet& packet);
    };

    class InterleavedStream {
        ll::OggSync m_sync;
        AutoStream m_s1, m_s2;
        AutoStream &m_theora, &m_vorbis;

    public:
        InterleavedStream(const std::string& data);

        AutoStream& theora() { return m_theora; }
        AutoStream& vorbis() { return m_vorbis; }

        bool operator>>(AutoStream& target);

    private:
        ogg_page getBOS();
        AutoStream& pickTheora();
        AutoStream& pickVorbis();
        AutoStream& pickStream(std::string_view type);
    };

    class VorbisDecoder {
        AutoStream& m_stream;
        ll::VorbisInfo m_info;
        ll::VorbisComment m_comment;
        ll::VorbisDecoder m_decoder;
        bool m_done;

    public:
        VorbisDecoder(InterleavedStream& source);

        const vorbis_info& info() { return m_info; }
        bool done() const { return m_done; }

        bool operator>>(std::vector<float>& buffer);

    private:
        vorbis_info* init();
    };

    class TheoraDecoder {
        AutoStream& m_stream;
        ll::TheoraInfo m_info;
        ll::TheoraComment m_comment;
        ll::TheoraSetup m_setup;
        ll::TheoraDecoder m_decoder;
        bool m_done;
        bool m_skipping;

    public:
        struct Frame {
            std::unique_ptr<std::uint8_t[]> yData;
            std::unique_ptr<std::uint8_t[]> cbData;
            std::unique_ptr<std::uint8_t[]> crData;
            float time;
        };

        TheoraDecoder(InterleavedStream& source);

        const th_info& info() { return m_info; }
        bool done() const { return m_done; }

        bool operator>>(Frame& frame);

        void skipToKey();

    private:
        void copy(std::unique_ptr<std::uint8_t[]>& dst, th_img_plane& src, int width, int height);
    };

}

#endif //FISH_FILLETS_OGG_VIDEO_H
