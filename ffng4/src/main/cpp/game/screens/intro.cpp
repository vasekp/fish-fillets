#include "intro.h"
#include "screenmanager.h"
#include "subsystem/files.h"

IntroScreen::IntroScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_ogg(instance.files().system("video/intro.ogv")->read()),
    m_vorbis(m_ogg),
    m_theora(m_ogg),
    m_aBuffer(std::make_shared<AudioSourceQueue>("intro audio"))
{
    auto& info = m_theora.info();
    if(info.pic_width != 640 || info.pic_height != 480)
        Log::fatal("Video expected in 640x480!");
    if(info.pixel_fmt != TH_PF_420)
        Log::fatal("Video expected in 4:2:0 Y'CbCr!");
    fill_buffers();
}

void IntroScreen::fill_buffers() {
    while(m_aBuffer->total() < (timeAlive() + 1) * 22050) {
        std::vector<float> aData;
        if(!(m_vorbis >> aData)) {
            Log::debug("Audio EOS");
            break;
        }
        Log::debug("Audio data: ", aData.size(), " frames");
        m_aBuffer->enqueue(std::move(aData));
    }

    while(m_vBuffer.size() < vBufSize) {
        m_vBuffer.emplace_back();
        auto& frame = m_vBuffer.back();
        if(!(m_theora >> frame)) {
            Log::debug("Video EOS");
            break;
        }
        Log::debug("Video frame @ ", frame.time);
    }
}

void IntroScreen::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_aBuffer);
    m_instance.screens().announceLevel("");
}

void IntroScreen::own_draw(const DrawTarget& target, float dt) {
    while(!m_vBuffer.empty() && m_vBuffer.front().time < timeAlive())
        m_vBuffer.pop_front();
    if(m_vBuffer.empty()) {
        Log::info("Video ended.");
        m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
    }
    auto& frame = m_vBuffer.front();
    Log::debug("drawing frame ", frame.time, " @ ", timeAlive());
    // TODO: upload upfront / asynchronously somehow? I think it doesn't really matter as we have to do 3 texture uploads
    // per frame anyway. Perhaps in the future when we have a mmapped GPU memory / pixel buffer object, but that does not
    // exist in the targeted OpenGL ES version.
    auto texY = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 640, 480, 640, frame.yData.data(), 1);
    auto texCb = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 320, 240, 320, frame.cbData.data(), 1);
    auto texCr = ogl::Texture::fromImageData(m_instance.graphics().system().ref(), 320, 240, 320, frame.crData.data(), 1);
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
