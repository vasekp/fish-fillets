#ifndef FISH_FILLETS_OGG_VIDEO_H
#define FISH_FILLETS_OGG_VIDEO_H

using namespace std::string_view_literals; // TODO global

namespace ogg {

    using namespace ll;

    class InterleavedStream {
    public:
        OggSync m_sync;
        std::array<OggStream, 2> m_streams;

    public:
        InterleavedStream(const std::string& data) :
            m_sync(data),
            m_streams(parseHeaders())
        { }

        OggStream& audio() { return m_streams[0]; }

        OggStream& video() { return m_streams[1]; }

        bool operator>>(OggStream& target) {
            ogg_page page;
            while(m_sync >> page) {
                for(auto& stream : m_streams)
                    if(stream << page && &stream == &target)
                        return true;
            }
            return false;
        }

    private:
        std::array<OggStream, 2> parseHeaders() {
            enum Order { AV, VA } order;

            ogg_page page;
            m_sync >> page;
            if(!ogg_page_bos(&page))
                Log::fatal("Not an Ogg video file (page 1)");
            if(std::string_view type((char*)page.body + 1, 6); type == "theora")
                order = VA;
            else if(type == "vorbis")
                order = AV;
            else
                Log::fatal("Ogg logical stream not recognized (page 1)");
            ogg_stream_state s1;
            //OggStream s1(ogg_page_serialno(&page));

            m_sync >> page;
            if(!ogg_page_bos(&page))
                Log::fatal("Not an Ogg video file (page 2)");
            if(std::string_view type((char*)page.body + 1, 6);
                    !((type == "theora" && order == AV) || (type == "vorbis" && order == VA)))
                Log::fatal("Not an Ogg video file (theora + vorbis not found)");
            //OggStream s2(ogg_page_serialno(&page));
            ogg_stream_state s2;

            return //order == AV ? std::array{s1, s2} : std::array{s2, s1};
            {OggStream{std::move(s1)}, OggStream{std::move(s2)}};
        }
    };

}

#endif //FISH_FILLETS_OGG_VIDEO_H
