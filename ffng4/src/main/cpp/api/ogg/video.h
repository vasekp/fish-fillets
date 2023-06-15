#ifndef FISH_FILLETS_OGG_VIDEO_H
#define FISH_FILLETS_OGG_VIDEO_H

namespace ogg {

    class InterleavedStream;

    class AutoStream : public ll::OggStream {
        InterleavedStream& m_source;

    public:
        AutoStream(InterleavedStream& source, ogg_page&& page) :
            ll::OggStream(page),
            m_source(source)
        { }

        bool operator>>(ogg_packet& packet);
    };

    class InterleavedStream {
        ll::OggSync m_sync;
        AutoStream m_s1, m_s2;
        AutoStream &m_theora, &m_vorbis;

    public:
        InterleavedStream(const std::string& data) :
            m_sync(data),
            m_s1(*this, getBOS()), m_s2(*this, getBOS()),
            m_theora(pickTheora()), m_vorbis(pickVorbis())
        { }

        AutoStream& theora() { return m_theora; }

        AutoStream& vorbis() { return m_vorbis; }

        bool operator>>(AutoStream& target) {
            ogg_page page;
            while(m_sync >> page) {
                for(auto* stream : {&m_s1, &m_s2})
                    if(*stream << page && stream == &target)
                        return true;
            }
            return false;
        }

    private:
        ogg_page getBOS() {
            ogg_page page;
            m_sync >> page;
            if(!ogg_page_bos(&page))
                Log::fatal("Expectation not met: Ogg BOS page");
            return page;
        }

        AutoStream& pickTheora() {
            return pickStream("theora");
        }

        AutoStream& pickVorbis() {
            return pickStream("vorbis");
        }

        AutoStream& pickStream(std::string_view type) {
            ogg_packet packet;
            ogg_stream_packetpeek(&m_s1, &packet);
            std::string_view type1((char*)packet.packet + 1, 6);
            if(type1 == type)
                return m_s1;
            ogg_stream_packetpeek(&m_s2, &packet);
            std::string_view type2((char*)packet.packet + 1, 6);
            if(type2 == type)
                return m_s2;
            Log::fatal("Stream ", type, " not found");
        }
    };

    inline bool AutoStream::operator>>(ogg_packet& packet) {
        if(int ret = ogg_stream_packetout(this, &packet); ret == 1)
            return true;
        else if(ret == 0) { // incomplete
            if(m_source >> *this)
                return operator>>(packet);
            else // end of stream
                return false;
        } else
            Log::fatal("Stream lost sync.");
    }

    class VorbisDecoder {
        AutoStream& m_stream;
        ll::VorbisInfo m_info;
        ll::VorbisComment m_comment;
        ll::VorbisDecoder m_decoder;

    public:
        VorbisDecoder(InterleavedStream& source) :
            m_stream(source.vorbis()),
            m_decoder(init())
        {
            Log::debug("Audio: ", m_info.channels, " channels, ", m_info.rate, " Hz");
        }

        const vorbis_info& info() {
            return m_info;
        }

        bool operator>>(std::vector<float>& buffer) {
            ogg_packet packet;
            if(!(m_stream >> packet))
                return false;
            m_decoder << packet;
            m_decoder >> buffer;
            return true;
        }

    private:
        vorbis_info* init() {
            ogg_packet packet;
            for(int i = 0; i < 3; i++) {
                m_stream >> packet;
                if(vorbis_synthesis_headerin(&m_info, &m_comment, &packet) < 0)
                    Log::error("Error parsing Vorbis headers");
            }
            return &m_info;
        }
    };

    class TheoraDecoder {
        AutoStream& m_stream;
        ll::TheoraInfo m_info;
        ll::TheoraComment m_comment;
        ll::TheoraSetup m_setup;
        ll::TheoraDecoder m_decoder;
        //bool started;

    public:
        struct Frame {
            std::vector<unsigned char> yData;
            std::vector<unsigned char> cbData;
            std::vector<unsigned char> crData;
            float time;
        };

        TheoraDecoder(InterleavedStream& source) :
            m_stream(source.theora())
            //started(false)
        {
            ogg_packet packet;
            for(int i = 0; i < 3; i++) {
                m_stream >> packet;
                if(th_decode_headerin(&m_info, &m_comment, &m_setup, &packet) < 0)
                    Log::error("Error parsing Theora headers");
            }
            auto fmtString = [&](th_pixel_fmt fmt) {
                return fmt == TH_PF_420 ? "4:2:0" : fmt == TH_PF_422 ? "4:2:2" : fmt == TH_PF_444 ? "4:4:4" : "unknown sampling";
            };
            m_decoder.set(th_decode_alloc(&m_info, m_setup));
            Log::debug("Video: ", m_info.pic_width, 'x', m_info.pic_height, ' ',
                    (float)m_info.fps_numerator / m_info.fps_denominator, "fps, ",
                    fmtString(m_info.pixel_fmt));
        }

        const th_info& info() {
            return m_info;
        }

        bool operator>>(Frame& frame) {
            ogg_packet packet;
            if(!(m_stream >> packet))
                return false;
            //if(!started) {
            th_decode_ctl(m_decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
            //    started = true;
            //}
            ogg_int64_t granulepos;
            if(th_decode_packetin(m_decoder, &packet, &granulepos) == 0) {
                auto time = th_granule_time(m_decoder, granulepos);
                Log::debug("video packet in @ ", time);
                th_ycbcr_buffer ycbcr;
                th_decode_ycbcr_out(m_decoder, ycbcr);
                frame.time = time;
                copy(frame.yData, ycbcr[0], 640, 480);
                copy(frame.cbData, ycbcr[1], 320, 240);
                copy(frame.crData, ycbcr[2], 320, 240);
            }
            return true;
        }

    private:
        void copy(std::vector<unsigned char>& dst, th_img_plane& src, int width, int height) {
            dst.resize(width * height);
            if(src.stride == width)
                std::memcpy(dst.data(), src.data, width * height);
            else {
                unsigned char* srcp = src.data;
                unsigned char* dstp = dst.data();
                for(int y = 0; y < height; y++) {
                    std::memcpy(dstp, srcp, width);
                    srcp += src.stride;
                    dstp += width;
                }
            }
        }
    };

}

#endif //FISH_FILLETS_OGG_VIDEO_H