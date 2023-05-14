#include <vorbis/codec.h>
#include <theora/theoradec.h>
#include <cstring>

template<class C, auto init, auto clear>
class OggStruct {
    C m_native;

public:
    template<typename... Ts>
    OggStruct(Ts... ts) { if constexpr(init != nullptr) init(&m_native, ts...); }
    OggStruct(C&& native) { std::memmove(&m_native, &native, sizeof(C)); }
    ~OggStruct() { clear(&m_native); }

    C* native() { return &m_native; }
    operator C*() { return native(); }
    C* operator->() { return native(); }
};

template<class C, auto free>
class OggPointer {
    C* m_native;

public:
    OggPointer(C* native = nullptr) { m_native = native; }
    ~OggPointer() { free(m_native); }

    C*& native() { return m_native; }
    C** operator&() { return &m_native; }
    operator C*() { return native(); }
};

using VorbisInfo = OggStruct<vorbis_info, vorbis_info_init, vorbis_info_clear>;
using VorbisComment = OggStruct<vorbis_comment, vorbis_comment_init, vorbis_comment_clear>;
using TheoraInfo = OggStruct<th_info, th_info_init, th_info_clear>;
using TheoraComment = OggStruct<th_comment, th_comment_init, th_comment_clear>;
using TheoraSetup = OggPointer<th_setup_info, th_setup_free>;
using TheoraDecoder = OggPointer<th_dec_ctx, th_decode_free>;
using VorbisBlock = OggStruct<vorbis_block, nullptr, vorbis_block_clear>;

class OggPage {
    ogg_page m_native;

public:
    OggPage(ogg_page&& page) { std::memmove(&m_native, &page, sizeof(ogg_page)); }

    operator ogg_page*() { return &m_native; }
    bool bos() { return ogg_page_bos(&m_native); }
    int serialno() { return ogg_page_serialno(&m_native); }
};

class OggSync : public OggStruct<ogg_sync_state, ogg_sync_init, ogg_sync_clear> {
public:
    void write(char* data, int size) {
        auto buffer = ogg_sync_buffer(native(), size);
        std::memcpy(buffer, data, size);
        ogg_sync_wrote(native(), size);
    }

    std::optional<OggPage> pageout() {
        ogg_page page;
        if(ogg_sync_pageout(native(), &page))
            return {std::move(page)};
        else
            return {};
    }
};

class OggStream : public OggStruct<ogg_stream_state, ogg_stream_init, ogg_stream_clear> {
public:
    int pagein(OggPage& page) { return ogg_stream_pagein(native(), page); }

    std::optional<ogg_packet> packetout() {
        ogg_packet packet;
        if(int ret = ogg_stream_packetout(native(), &packet); ret > 0)
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
        vorbis_block_init(native(), &block);
        return {std::move(block)};
    }
};
