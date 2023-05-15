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
    if(m_thInfo->pic_width != 640 || m_thInfo->pic_height != 480)
        Log::fatal("Video expected in 640x480!");
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
    if(fmt != TH_PF_420)
        Log::fatal("Video expected in 4:2:0 Y'CbCr!");
    // TODO postprocessing level?
    m_vbDecoder = std::make_unique<VorbisDecoder>(m_vbInfo.native());
    m_vbBlock = std::make_unique<VorbisBlock>(m_vbDecoder->block());
    Log::debug("Audio: ", m_vbInfo->channels, " channels, ", m_vbInfo->rate, " Hz");
    if(m_vbInfo->rate != 22050 || m_vbInfo->channels != 1)
        Log::fatal("Audio expected in 22050 Hz, mono!");

    fill_buffers();
}

void IntroScreen::more_data() { // TODO eof
    m_oggSync.write(m_data.data() + m_offset, 4096);
    m_offset += 4096;
}

void IntroScreen::queue_page(OggPage& page) {
    if(m_vbStream)
        if(m_vbStream->pagein(page) == 0)
            Log::debug("Vorbis page");
    if(m_thStream)
        if(m_thStream->pagein(page) == 0)
            Log::debug("Theora page");
}

void IntroScreen::fill_buffers() {
    for(;;) {
        std::vector<float> adata{};
        for(;;) {
            if(auto ret = m_vbDecoder->pcmout(); ret.size() != 0) {
                //Log::debug("audio packet size ", ret.size());
                adata.reserve(adata.size() + ret.size());
                adata.insert(adata.end(), ret.begin(), ret.end());
                m_vbDecoder->read(ret.size());
            } else {
                if(auto packet = m_vbStream->packetout()) {
                    if(m_vbBlock->synthesis(&*packet) == 0)
                        m_vbDecoder->blockin(*m_vbBlock);
                } else
                    break;
            }
        }
        if(!adata.empty()) {
            Log::debug("audio data: ", adata.size(), " frames");
            m_aBuffer.push_back(std::move(adata));
        }
        while(m_vBuffer.size() < 2) {
            if(auto packet = m_thStream->packetout()) {
                std::int64_t granulepos;
                if(m_thDecoder->packetin(&*packet, &granulepos) == 0) {
                    auto time = th_granule_time(*m_thDecoder, granulepos);
                    Log::debug("video packet in @ ", time);
                    th_ycbcr_buffer ycbcr;
                    th_decode_ycbcr_out(*m_thDecoder, ycbcr);
                    /*Log::debug("YUV ",
                        ycbcr[0].width, 'x', ycbcr[0].height, '%', ycbcr[0].stride, ' ',
                        ycbcr[1].width, 'x', ycbcr[1].height, '%', ycbcr[1].stride, ' ',
                        ycbcr[2].width, 'x', ycbcr[2].height, '%', ycbcr[2].stride);*/
                    m_vBuffer.emplace_back();
                    Frame& frame = m_vBuffer.back();
                    frame.time = time;
                    auto copy = [&](th_img_plane& src, auto& dst, int width, int height) {
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
                    };
                    copy(ycbcr[0], frame.data_y, 640, 480);
                    copy(ycbcr[1], frame.data_cb, 320, 240);
                    copy(ycbcr[2], frame.data_cr, 320, 240);
                }
            } else
                break;
        }
        if(m_vBuffer.size() < 2 || m_aBuffer.empty()) {
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
    if(m_vBuffer.size() == 2 && m_vBuffer.back().time < timeAlive())
        m_vBuffer.pop_front();
    if(m_vBuffer.empty())
        return; // TODO quit screen
    Frame& frame = m_vBuffer.front();
    Log::debug("drawing frame ", frame.time, " @ ", timeAlive());
    auto texY = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 640, 480, 640, frame.data_y.data(), 1);
    auto texCb = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 320, 240, 320, frame.data_cb.data(), 1);
    auto texCr = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 320, 240, 320, frame.data_cr.data(), 1);
    const auto& program = m_instance.graphics().shaders().ycbcr;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    glActiveTexture(Shaders::texCb_gl);
    glBindTexture(GL_TEXTURE_2D, texCb);
    glActiveTexture(Shaders::texCr_gl);
    glBindTexture(GL_TEXTURE_2D, texCr);
    glActiveTexture(Shaders::texImage_gl);
    target.blit(texY, coords, program);
    fill_buffers();
}

bool IntroScreen::own_pointer(FCoords coords, bool longPress) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool IntroScreen::own_key(Key key) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}
