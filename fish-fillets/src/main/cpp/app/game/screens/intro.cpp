#include "intro.h"
#include "screenmanager.h"
#include "subsystem/files.h"

IntroScreen::IntroScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_ogg(instance.files().system("video/intro.ogv")->read()),
    m_vorbis(m_ogg),
    m_theora(m_ogg),
    m_aBuffer(std::make_shared<AudioSourceQueue>("intro audio", AudioType::music)),
    m_texTime(-1.f)
{
    auto& info = m_theora.info();
    if(info.pic_width != 640 || info.pic_height != 480)
        Log::fatal("Video expected in 640x480!");
    if(info.pixel_fmt != TH_PF_420)
        Log::fatal("Video expected in 4:2:0 Y'CbCr!");
    fill_buffers();
}

void IntroScreen::fill_buffers() {
    while(m_aBuffer->total() < (size_t)((timeAlive() + 1) * 22050)) {
        std::vector<float> aData;
        if(!(m_vorbis >> aData)) {
            Log::debug<Log::video>("Audio EOS");
            break;
        }
        Log::verbose<Log::video>("Audio data: ", aData.size(), " frames");
        m_aBuffer->enqueue(std::move(aData));
    }

    if(!m_vBuffer.empty() && !m_theora.done() && m_vBuffer.back().time < timeAlive()) {
        Log::warn("Video lagging by ", timeAlive() - m_vBuffer.back().time, "s, skipping to next keyframe, ",
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

void IntroScreen::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_aBuffer);
    m_instance.screens().announceLevel("");
}

void IntroScreen::own_update() {
    while(m_vBuffer.size() > 1 && m_vBuffer.front().time < timeAlive())
        m_vBuffer.pop_front();
    if(m_vBuffer.size() == 1 && m_vBuffer.front().time < timeAlive() && m_theora.done()) {
        Log::debug<Log::video>("Intro ended.");
        m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    }
    // FIXME: Condiser upload upfront / asynchronously somehow? I think it doesn't really matter as we have to do 3 texture uploads
    // per frame anyway. Perhaps in the future when we have a mmapped GPU memory / pixel buffer object, but that does not
    // exist in the targeted OpenGL ES version.
    auto& frame = m_vBuffer.front();
    if(frame.time == m_texTime)
        return;
    Log::verbose<Log::video>("uploading frame ", frame.time);
    m_texY = Texture(m_instance.graphics().system(), frame.yData.data(), {640, 480}, 1);
    m_texCb = Texture(m_instance.graphics().system(), frame.cbData.data(), {320, 240}, 1);
    m_texCr = Texture(m_instance.graphics().system(), frame.crData.data(), {320, 240}, 1);
    m_texTime = frame.time;
    fill_buffers();
}

void IntroScreen::own_draw(DrawTarget& target) {
    Log::verbose<Log::video>("drawing frame ", m_texTime, " @ ", timeAlive());
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto program = m_instance.graphics().shaders().YCbCr({
        .texY = m_texY,
        .texCb = m_texCb,
        .texCr = m_texCr,
    });
    target.draw(program, coords, { .area = Graphics::baseDim });
}

bool IntroScreen::own_pointer(FCoords coords) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}

bool IntroScreen::own_key(Key key) {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    return true;
}
