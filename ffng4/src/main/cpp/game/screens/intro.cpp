#include "intro.h"
#include "screenmanager.h"
#include "subsystem/files.h"

IntroScreen::IntroScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_oggSync(),
    m_data(instance.files().system("video/intro.ogv")->read()), // TODO
    m_offset(0)
{
    /* parse headers and find audio and video streams */
    [&]() {
        for(;;) {
            more_data();
            while(auto page = m_oggSync.pageout()) {
                if(!page->bos()) {
                    queue_page(*page);
                    return;
                }
                std::unique_ptr<OggStream> stream = std::make_unique<OggStream>(page->serialno());
                stream->pagein(*page);
                auto packet = stream->packetout();
                if(!m_thStream && th_decode_headerin(m_thInfo, m_thComment, &m_thSetup, &*packet) >= 0)
                    m_thStream = std::move(stream);
                else if(!m_vbStream && vorbis_synthesis_headerin(m_vbInfo, m_vbComment, &*packet) >= 0)
                    m_vbStream = std::move(stream);
                else
                    Log::warn("discarding ogg stream");
            }
        }
    }();
    if(!m_thStream)
        Log::fatal("Theora stream not found in intro.ogv");
    if(!m_vbStream)
        Log::fatal("Vorbis stream not found in intro.ogv");
    Log::debug("intro: audio & video streams found");

    /* parse header packets */
    for(int vbPackets = 1, thPackets = 1; vbPackets < 3 || thPackets < 3; ) {
        std::optional<ogg_packet> packet;
        while(vbPackets < 3 && (packet = m_vbStream->packetout())) {
            if(vorbis_synthesis_headerin(m_vbInfo, m_vbComment, &*packet) < 0)
                Log::fatal("Error parsing Vorbis headers");
            else
                vbPackets++;
        }
        while(thPackets < 3 && (packet = m_thStream->packetout())) {
            if(th_decode_headerin(m_thInfo, m_thComment, &m_thSetup, &*packet) < 0)
                Log::fatal("Error parsing Theora headers");
            else
                thPackets++;
        }
        if(auto page = m_oggSync.pageout())
            queue_page(*page);
        else
            more_data();
    }
    Log::debug("intro: packet parsing successful");

    /* Initialize decoders */
    m_thDecoder = std::make_unique<TheoraDecoder>(th_decode_alloc(m_thInfo, m_thSetup));
    Log::debug("Video: ", m_thInfo->pic_width, 'x', m_thInfo->pic_height,
            ' ', (float)m_thInfo->fps_numerator / m_thInfo->fps_denominator, "fps");
    // TODO check 640x480?
    th_pixel_fmt fmt = m_thInfo->pixel_fmt;
    switch(fmt) {
        case TH_PF_420:
            Log::debug("Video: 4:2:0");
            break;
        case TH_PF_422:
            Log::debug("Video: 4:2:2");
            break;
        case TH_PF_444:
            Log::debug("Video: 4:4:4");
            break;
        default:
            Log::error("Video: unknown sampling");
    }
    // TODO postprocessing level?
    m_vbDecoder = std::make_unique<VorbisDecoder>(m_vbInfo.native());
    m_vbBlock = std::make_unique<VorbisBlock>(m_vbDecoder->block());
    Log::debug("Audio: ", m_vbInfo->channels, " channels, ", m_vbInfo->rate, " Hz");
    // TODO check 22050, 1 channel

    fill_buffers();
}

void IntroScreen::more_data() {
    m_oggSync.write(m_data.data() + m_offset, 4096);
    m_offset += 4096;
}

void IntroScreen::queue_page(OggPage& page) {
    if(m_vbStream)
        m_vbStream->pagein(page);
    if(m_thStream)
        m_thStream->pagein(page);
}

void IntroScreen::fill_buffers() {
    bool vframe = false;
    bool aframe = false;
    for(;;) {
        while(!aframe) {
            if(auto ret = m_vbDecoder->pcmout(); ret.size() != 0) {
                Log::debug("audio packet size ", ret.size());
                aframe = true;
            } else {
                if(auto packet = m_vbStream->packetout()) {
                    if(m_vbBlock->synthesis(&*packet) == 0)
                        m_vbDecoder->blockin(*m_vbBlock);
                } else
                    break;
            }
        }
        while(!vframe) {
            if(auto packet = m_thStream->packetout()) {
                if(m_thDecoder->packetin(&*packet, nullptr) == 0) {
                    Log::debug("video packet in");
                    vframe = true;
                }
            } else
                break;
        }
        if(!vframe || !aframe) {
            more_data();
            while(auto page = m_oggSync.pageout())
                queue_page(*page);
        } else
            break;
    }
}

void IntroScreen::own_start() {
    m_instance.audio().clear();
    m_instance.screens().announceLevel("");
}

void IntroScreen::own_draw(const DrawTarget& target, float dt) {
}

bool IntroScreen::own_pointer(FCoords coords, bool longPress) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool IntroScreen::own_key(Key key) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}
