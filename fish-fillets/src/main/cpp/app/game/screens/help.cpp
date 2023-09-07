#include "help.h"
#include "screenmanager.h"
#include "subsystem/files.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"

static constexpr std::size_t vBufSize = 5;

HelpScreen::HelpScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_demux(instance.files().system("video/navod1.ogv")->read(), 1),
    m_theora(m_demux),
    m_imgY(BufferImage::create(instance, {640, 480}, TextureType::channelY, nullptr)),
    m_imgCb(BufferImage::create(instance, {320, 240}, TextureType::channelCb, nullptr)),
    m_imgCr(BufferImage::create(instance, {320, 240}, TextureType::channelCr, nullptr)),
    m_imgTime(-1.f),
    m_startTime(0.f)
{
    auto& info = m_theora.info();
    if(info.pic_width != 640 || info.pic_height != 480)
        Log::fatal("Video expected in 640x480!");
    if(info.pixel_fmt != TH_PF_420)
        Log::fatal("Video expected in 4:2:0 Y'CbCr!");
    fill_buffers();
}

float HelpScreen::timeSinceStart() {
    return timeAlive() - m_startTime;
}

void HelpScreen::fill_buffers() {
    if(!m_vBuffer.empty() && !m_theora.done() && m_vBuffer.back().time < timeSinceStart()) {
        Log::warn("Video lagging by ", timeSinceStart() - m_vBuffer.back().time, "s, skipping to next keyframe, ",
            "dropping ", m_vBuffer.size(), " decoded frames");
        m_vBuffer.clear();
        m_theora.skipToKey();
    }

    while(m_vBuffer.size() < vBufSize) {
        m_vBuffer.emplace_back();
        auto& frame = m_vBuffer.back();
        if(!(m_theora >> frame)) {
            Log::debug<Log::video>("Video EOS");
            m_vBuffer.pop_back();
            break;
        }
        Log::verbose<Log::video>("Video frame @ ", frame.time);
    }
}

void HelpScreen::own_start() { }

void HelpScreen::own_update() {
    while(m_vBuffer.size() > 1 && m_vBuffer.front().time < timeSinceStart())
        m_vBuffer.pop_front();
    if(m_vBuffer.size() == 1 && m_vBuffer.front().time < timeSinceStart() && m_theora.done()) {
        m_demux.reset();
        m_theora.reset();
        m_vBuffer.clear();
        Log::debug<Log::video>("video loop");
        m_startTime = timeAlive();
        fill_buffers();
    }
    auto& frame = m_vBuffer.front();
    if(frame.time == m_imgTime)
        return;
    Log::verbose<Log::video>("uploading frame ", frame.time);
    dynamic_cast<BufferImage*>(*m_imgY)->replace(std::move(frame.yData));
    dynamic_cast<BufferImage*>(*m_imgCb)->replace(std::move(frame.cbData));
    dynamic_cast<BufferImage*>(*m_imgCr)->replace(std::move(frame.crData));
    m_imgTime = frame.time;
    fill_buffers();
}

void HelpScreen::own_draw(DrawTarget& target) {
    Log::verbose<Log::video>("drawing frame ", m_imgTime, " @ ", timeAlive());
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto program = m_instance.graphics().shaders().YCbCr();
    target.draw({
        m_imgY->texture(), m_imgCb->texture(), m_imgCr->texture()
    }, program, coords);
}

bool HelpScreen::own_pointer(FCoords coords) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool HelpScreen::own_key(Key key) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}
