#ifndef FISH_FILLETS_OGG_H
#define FISH_FILLETS_OGG_H

#include "common.h"

#include <vorbis/codec.h>
#include <theora/theoradec.h>

#include "ogg/internals.h"

namespace ogg {

    using namespace internal;

    using VorbisInfo = OggStruct<vorbis_info, vorbis_info_init, vorbis_info_clear>;
    using VorbisComment = OggStruct<vorbis_comment, vorbis_comment_init, vorbis_comment_clear>;
    using VorbisBlock = OggStruct<vorbis_block, nullptr, vorbis_block_clear>;
    using TheoraInfo = OggStruct<th_info, th_info_init, th_info_clear>;
    using TheoraComment = OggStruct<th_comment, th_comment_init, th_comment_clear>;

    using TheoraSetup = OggPointer<th_setup_info, th_setup_free>;
    using TheoraDecoder = OggPointer<th_dec_ctx, th_decode_free>;

    class OggSync : public OggStruct<ogg_sync_state, ogg_sync_init, ogg_sync_clear> {
    public:
        void write(char* data, int size) {
            auto buffer = ogg_sync_buffer(this, size);
            std::memcpy(buffer, data, size);
            ogg_sync_wrote(this, size);
        }

        std::optional<ogg_page> pageout() {
            ogg_page page;
            if(ogg_sync_pageout(this, &page))
                return page;
            else
                return {};
        }
    };

    class OggStream : public OggStruct<ogg_stream_state, ogg_stream_init, ogg_stream_clear> {
    public:
        int pagein(ogg_page* page) {
            return ogg_stream_pagein(this, page);
        }

        std::optional<ogg_packet> packetout() {
            ogg_packet packet;
            if(int ret = ogg_stream_packetout(this, &packet); ret > 0)
                return {packet};
            else if(ret < 0)
                Log::fatal("ogg_stream_packetout failed: ", ret);
            else
                return {};
        }
    };

    class VorbisDecoder : public OggStruct<vorbis_dsp_state, vorbis_synthesis_init, vorbis_dsp_clear> {
    public:
        VorbisBlock block() {
            vorbis_block block;
            vorbis_block_init(this, &block);
            return {std::move(block)};
        }

        void blockin(vorbis_block *block) {
            vorbis_synthesis_blockin(this, block);
        }

        std::vector<float> pcmout() {
            float **pcm;
            auto size = vorbis_synthesis_pcmout(this, &pcm);
            std::vector<float> ret{};
            ret.resize(size);
            std::memcpy(ret.data(), pcm[0], size * sizeof(float));
            return ret;
        }

        void read(int samples) {
            vorbis_synthesis_read(this, samples);
        }
    };

}

#endif //FISH_FILLETS_OGG_H
