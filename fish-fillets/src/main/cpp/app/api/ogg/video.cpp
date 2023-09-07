#include "video.h"

namespace ogg {

    AutoStream::AutoStream(DemuxStream& source, ogg_page&& page) :
            ll::OggStream(page),
            m_source(source)
        { }

    bool AutoStream::operator>>(ogg_packet& packet) {
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

    DemuxStream::DemuxStream(const std::string& data, std::size_t numStreams) :
        m_sync(data)
    {
        for(auto i = 0u; i < numStreams; i++) {
            auto stream = std::make_unique<AutoStream>(*this, getBOS());
            ogg_packet packet;
            ogg_stream_packetpeek(&*stream, &packet);
            std::string type((char*)packet.packet + 1, 6);
            m_streams.emplace_back(std::move(type), std::move(stream));
        }
    }

    bool DemuxStream::operator>>(AutoStream& target) {
        ogg_page page;
        while(m_sync >> page) {
            for(auto& [type, stream] : m_streams)
                if(*stream << page && stream.get() == &target)
                    return true;
        }
        return false;
    }

    ogg_page DemuxStream::getBOS() {
        ogg_page page;
        m_sync >> page;
        if(!ogg_page_bos(&page))
            Log::fatal("Expectation not met: Ogg BOS page");
        return page;
    }

    AutoStream& DemuxStream::findStream(std::string_view type) {
        for(auto& [type1, stream] : m_streams)
            if(type1 == type)
                return *stream;
        Log::fatal("Stream ", type, " not found");
    }

    void DemuxStream::reset() {
        auto fill = m_sync.fill;
        ogg_sync_reset(&m_sync);
        ogg_sync_wrote(&m_sync, fill);
        for(auto& [type, stream] : m_streams)
            ogg_stream_reset(&*stream);
    }

    VorbisDecoder::VorbisDecoder(DemuxStream& source) :
        m_stream(source.findStream("vorbis")),
        m_decoder(init()),
        m_done(false)
    {
        Log::debug<Log::video>("Audio: ", m_info.channels, " channels, ", m_info.rate, " Hz");
    }


    bool VorbisDecoder::operator>>(std::vector<float>& buffer) {
        ogg_packet packet;
        if(!(m_stream >> packet)) {
            m_done = true;
            return false;
        }
        m_decoder << packet;
        m_decoder >> buffer;
        return true;
    }

    vorbis_info* VorbisDecoder::init() {
        ogg_packet packet;
        for(int i = 0; i < 3; i++) {
            m_stream >> packet;
            if(vorbis_synthesis_headerin(&m_info, &m_comment, &packet) < 0)
                Log::error("Error parsing Vorbis headers");
        }
        return &m_info;
    }

    TheoraDecoder::TheoraDecoder(DemuxStream& source) :
        m_stream(source.findStream("theora")),
        m_done(false),
        m_skipping(false)
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
        Log::debug<Log::video>("Video: ", m_info.pic_width, 'x', m_info.pic_height, ' ',
                (float)m_info.fps_numerator / m_info.fps_denominator, "fps, ",
                fmtString(m_info.pixel_fmt));
    }

    void TheoraDecoder::skipToKey() {
        m_skipping = true;
    }

    void TheoraDecoder::reset() {
        m_done = false;
        m_skipping = false;
        ogg_packet packet;
        for(int i = 0; i < 3; i++)
            m_stream >> packet;
    }

    bool TheoraDecoder::operator>>(TheoraDecoder::Frame& frame) {
        ogg_packet packet;
        int skipped = 0;
        do {
            if(!(m_stream >> packet)) {
                m_done = true;
                return false;
            }
            if(m_skipping) {
                if(packet.granulepos >= 0 && (packet.granulepos & 63) == 0)
                    m_skipping = false;
                else
                    skipped++;
            }
        } while(m_skipping);
        if(skipped)
            Log::warn("Skipped ", skipped, " frames!");
        th_decode_ctl(m_decoder, TH_DECCTL_SET_GRANPOS, &packet.granulepos, sizeof(packet.granulepos));
        if(ogg_int64_t granulepos; th_decode_packetin(m_decoder, &packet, &granulepos) == 0) {
            auto time = th_granule_time(m_decoder, granulepos);
            Log::verbose<Log::video>("video packet in @ ", time, " granulepos ", granulepos >> 6, "+", granulepos & 63);
            th_ycbcr_buffer ycbcr;
            th_decode_ycbcr_out(m_decoder, ycbcr);
            frame.time = time;
            copy(frame.yData, ycbcr[0]);
            copy(frame.cbData, ycbcr[1]);
            copy(frame.crData, ycbcr[2]);
            return true;
        } else
            return false;
    }

    void TheoraDecoder::copy(std::unique_ptr<std::uint8_t[]>& dst, th_img_plane& src) {
        auto byteSize = src.width * src.height;
        dst = std::make_unique<std::uint8_t[]>(byteSize);
        if(src.stride == src.width)
            std::memcpy(dst.get(), src.data, byteSize);
        else {
            std::uint8_t* srcp = src.data;
            std::uint8_t* dstp = dst.get();
            for(auto y = 0; y < src.height; y++) {
                std::memcpy(dstp, srcp, src.width);
                srcp += src.stride;
                dstp += src.width;
            }
        }
    }

}
