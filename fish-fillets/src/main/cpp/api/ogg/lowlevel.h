#ifndef FISH_FILLETS_OGG_LOWLEVEL_H
#define FISH_FILLETS_OGG_LOWLEVEL_H

#include "internals.h"

namespace ogg::ll {

    using namespace ogg::internal;

    using VorbisInfo = OggStruct<vorbis_info, vorbis_info_init, vorbis_info_clear>;
    using VorbisComment = OggStruct<vorbis_comment, vorbis_comment_init, vorbis_comment_clear>;
    using TheoraInfo = OggStruct<th_info, th_info_init, th_info_clear>;
    using TheoraComment = OggStruct<th_comment, th_comment_init, th_comment_clear>;

    using TheoraSetup = OggPointer<th_setup_info, th_setup_free>;
    using TheoraDecoder = OggPointer<th_dec_ctx, th_decode_free>;

    class OggSync : public OggStruct<ogg_sync_state, ogg_sync_init, ogg_sync_clear> {
    public:
        OggSync(const std::string& data) {
            operator<<(data);
        }

        void operator<<(const std::string& data) {
            auto buffer = ogg_sync_buffer(this, data.size());
            std::memcpy(buffer, data.data(), data.size());
            ogg_sync_wrote(this, data.size());
        }

        bool operator>>(ogg_page& page) {
            return ogg_sync_pageout(this, &page) == 1;
        }
    };

    class OggStream : public OggStruct<ogg_stream_state, ogg_stream_init, ogg_stream_clear> {
    public:
        using OggStruct::OggStruct;

        OggStream(ogg_page& page) : OggStream(ogg_page_serialno(&page)) {
            operator<<(page);
        }

        bool operator<<(ogg_page& page) {
            return ogg_stream_pagein(this, &page) == 0;
        }

        bool operator>>(ogg_packet& packet) {
            return ogg_stream_packetout(this, &packet) == 1;
        }
    };

    class VorbisDecoder : public OggStruct<vorbis_dsp_state, vorbis_synthesis_init, vorbis_dsp_clear> {
        vorbis_block m_block;
    public:
        VorbisDecoder(vorbis_info* info) : OggStruct(info) {
            vorbis_block_init(this, &m_block);
        }

        ~VorbisDecoder() {
            vorbis_block_clear(&m_block);
        }

        void operator<<(ogg_packet& packet) {
            if(auto ret = vorbis_synthesis(&m_block, &packet); ret != 0) {
                Log::error("vorbis_synthesis returned ", ret);
                return;
            }
            if(auto ret = vorbis_synthesis_blockin(this, &m_block); ret != 0)
                Log::error("vorbis_synthesis_blockin returned ", ret);
        }

        std::size_t operator>>(std::vector<float>& ret) {
            float **pcm;
            auto oldSize = ret.size();
            auto size = vorbis_synthesis_pcmout(this, &pcm);
            Log::verbose<Log::video>("pcmout size: ", size, " vector ", (void*)(pcm), " ", (void*)(pcm[0]));
            ret.resize(oldSize + size);
            std::memcpy(ret.data() + oldSize, pcm[0], size * sizeof(float));
            vorbis_synthesis_read(this, size);
            return size;
        }
    };

}

#endif //FISH_FILLETS_OGG_LOWLEVEL_H
