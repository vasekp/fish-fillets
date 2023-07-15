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
        OggSync(const std::string& data);

        void operator<<(const std::string& data);
        bool operator>>(ogg_page& page);
    };

    class OggStream : public OggStruct<ogg_stream_state, ogg_stream_init, ogg_stream_clear> {
    public:
        using OggStruct::OggStruct;

        OggStream(ogg_page& page);

        bool operator<<(ogg_page& page);
        bool operator>>(ogg_packet& packet);
    };

    class VorbisDecoder : public OggStruct<vorbis_dsp_state, vorbis_synthesis_init, vorbis_dsp_clear> {
        vorbis_block m_block;
    public:
        VorbisDecoder(vorbis_info* info);
        ~VorbisDecoder();

        void operator<<(ogg_packet& packet);
        std::size_t operator>>(std::vector<float>& ret);
    };

}

#endif //FISH_FILLETS_OGG_LOWLEVEL_H
