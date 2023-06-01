#include <vorbis/codec.h>
#include <theora/theoradec.h>
#include <cstring>

template<class C, auto init, auto clear>
class OggStruct : public C {
public:
    template<typename... Ts>
    OggStruct(Ts... ts) { if constexpr(init != nullptr) init(this, ts...); }
    OggStruct(C&& nativeObj) { std::memmove(this, &nativeObj, sizeof(C)); }
    ~OggStruct() { if constexpr(clear != nullptr) clear(this); }
};

using VorbisInfo = OggStruct<vorbis_info, vorbis_info_init, vorbis_info_clear>;
using VorbisComment = OggStruct<vorbis_comment, vorbis_comment_init, vorbis_comment_clear>;
using TheoraInfo = OggStruct<th_info, th_info_init, th_info_clear>;
using TheoraComment = OggStruct<th_comment, th_comment_init, th_comment_clear>;

template<class C, auto free>
class OggPointer {
    C* m_native;

public:
    OggPointer() : m_native(nullptr) { }
    ~OggPointer() { if(m_native) free(m_native); }

    void set(C* ptr) {
        assert(!m_native);
        m_native = ptr;
    }

    C* native() { return m_native; }
    C** operator&() { return &m_native; }
    operator C*() { return native(); }
};

using TheoraSetup = OggPointer<th_setup_info, th_setup_free>;
using TheoraDecoder = OggPointer<th_dec_ctx, th_decode_free>;

class OggPage : public OggStruct<ogg_page, nullptr, nullptr> {
public:
    using OggStruct<ogg_page, nullptr, nullptr>::OggStruct;
    bool bos() { return ogg_page_bos(this); }
    int serialno() { return ogg_page_serialno(this); }
};

class OggSync : public OggStruct<ogg_sync_state, ogg_sync_init, ogg_sync_clear> {
public:
    void write(char* data, int size) {
        auto buffer = ogg_sync_buffer(this, size);
        std::memcpy(buffer, data, size);
        ogg_sync_wrote(this, size);
    }

    std::optional<OggPage> pageout() {
        ogg_page page;
        if(ogg_sync_pageout(this, &page))
            return {std::move(page)};
        else
            return {};
    }
};

class OggStream : public OggStruct<ogg_stream_state, ogg_stream_init, ogg_stream_clear> {
public:
    int pagein(ogg_page* page) { return ogg_stream_pagein(this, page); }

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

class VorbisBlock : public OggStruct<vorbis_block, nullptr, vorbis_block_clear> {
public:
    int synthesis(ogg_packet* packet) {
        return vorbis_synthesis(this, packet);
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

