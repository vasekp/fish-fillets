#ifndef FISH_FILLETS_OGG_VIDEO_H
#define FISH_FILLETS_OGG_VIDEO_H

#include "lowlevel.h"

namespace ogg {

    class DemuxStream;

    class AutoStream : public ll::OggStream {
    public:
        AutoStream(DemuxStream& source, ogg_page&& page);

        bool operator>>(ogg_packet& packet);

    private:
        DemuxStream& m_source;
    };

    class DemuxStream {
    public:
        DemuxStream(const std::string& data, std::size_t numStreams);

        AutoStream& findStream(std::string_view type);
        bool operator>>(AutoStream& target);
        void reset();

    private:
        ll::OggSync m_sync;
        std::vector<std::pair<std::string, std::unique_ptr<AutoStream>>> m_streams;

        ogg_page getBOS();
    };

    class VorbisDecoder {
    public:
        VorbisDecoder(DemuxStream& source);

        const vorbis_info& info() { return m_info; }
        bool done() const { return m_done; }

        bool operator>>(std::vector<float>& buffer);

    private:
        AutoStream& m_stream;
        ll::VorbisInfo m_info;
        ll::VorbisComment m_comment;
        ll::VorbisDecoder m_decoder;
        bool m_done;

        vorbis_info* init();
    };

    class TheoraDecoder {
    public:
        struct Frame {
            std::unique_ptr<std::uint8_t[]> yData;
            std::unique_ptr<std::uint8_t[]> cbData;
            std::unique_ptr<std::uint8_t[]> crData;
            float time;
        };

        TheoraDecoder(DemuxStream& source);

        const th_info& info() { return m_info; }
        bool done() const { return m_done; }

        bool operator>>(Frame& frame);

        void skipToKey();
        void reset();

    private:
        AutoStream& m_stream;
        ll::TheoraInfo m_info;
        ll::TheoraComment m_comment;
        ll::TheoraSetup m_setup;
        ll::TheoraDecoder m_decoder;
        bool m_done;
        bool m_skipping;

        void copy(std::unique_ptr<std::uint8_t[]>& dst, th_img_plane& src);
    };

}

#endif //FISH_FILLETS_OGG_VIDEO_H
