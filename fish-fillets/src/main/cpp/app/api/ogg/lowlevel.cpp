#include "lowlevel.h"

namespace ogg::ll {

    using namespace ogg::internal;

    OggSync::OggSync(const std::string& data) {
        operator<<(data);
    }

    void OggSync::operator<<(const std::string& data) {
        auto buffer = ogg_sync_buffer(this, data.size());
        std::memcpy(buffer, data.data(), data.size());
        ogg_sync_wrote(this, data.size());
    }

    bool OggSync::operator>>(ogg_page& page) {
        return ogg_sync_pageout(this, &page) == 1;
    }

    OggStream::OggStream(ogg_page& page) : OggStream(ogg_page_serialno(&page)) {
        operator<<(page);
    }

    bool OggStream::operator<<(ogg_page& page) {
        return ogg_stream_pagein(this, &page) == 0;
    }

    bool OggStream::operator>>(ogg_packet& packet) {
        return ogg_stream_packetout(this, &packet) == 1;
    }

    VorbisDecoder::VorbisDecoder(vorbis_info* info) : OggStruct(info) {
        vorbis_block_init(this, &m_block);
    }

    VorbisDecoder::~VorbisDecoder() {
        vorbis_block_clear(&m_block);
    }

    void VorbisDecoder::operator<<(ogg_packet& packet) {
        if(auto ret = vorbis_synthesis(&m_block, &packet); ret != 0) {
            Log::error("vorbis_synthesis returned ", ret);
            return;
        }
        if(auto ret = vorbis_synthesis_blockin(this, &m_block); ret != 0)
            Log::error("vorbis_synthesis_blockin returned ", ret);
    }

    std::size_t VorbisDecoder::operator>>(std::vector<float>& ret) {
        float **pcm;
        auto oldSize = ret.size();
        auto size = vorbis_synthesis_pcmout(this, &pcm);
        if(size == 0)
            return 0;
        Log::verbose<Log::video>("pcmout size: ", size, " vector ", (void*)(pcm), " ", (void*)(pcm[0]));
        ret.resize(oldSize + size);
        std::memcpy(ret.data() + oldSize, pcm[0], size * sizeof(float));
        vorbis_synthesis_read(this, size);
        return size;
    }

}
