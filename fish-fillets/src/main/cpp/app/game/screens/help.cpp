#include "help.h"
#include "screenmanager.h"
#include "subsystem/files.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "subsystem/persist.h"

static constexpr std::size_t vBufSize = 5;

static const std::array files{
    "video/navod1.ogv",
    "video/navod2.ogv",
    "video/navod3.ogv",
    "video/navod4.ogv",
    "video/navod5.ogv",
    "video/navod6.ogv",
    "video/navod7.ogv",
    "video/navod8.ogv",
    "video/navod9.ogv"
};

static const std::array help_cs{
    "Pohyb o políčko: potažení prstem",
    "Více pohybů jedním gestem",
    "Pokračování ve stejném směru: podržet",
    "Změna rybky: poklepání",
    "Poklepání na konkrétní rybku",
    "Přejetí na dosažitelnou polohu: podržet prst",
    "Urychlení pohybu: ťuknutí druhým prstem",
    "Přerušit: ťuknutí během pohybu",
    "Vrátit jeden krok: potažení dvěma prsty\nMožno pouze do dvou sekund od provedené akce!"
};

static const std::array help_en{
    "Single move: finger gesture",
    "Multiple moves in single gesture",
    "Continue in the same direction: hold",
    "Change fish: double tap",
    "Select specific fish",
    "Find path to accessible location: tap and hold",
    "Fast forward: tap with second finger",
    "Break running motion: tap anywhere",
    "Undo one move: two finger drag\nOnly possible within 2 seconds!"
};

static_assert(help_cs.size() == files.size());
static_assert(help_en.size() == files.size());

static constexpr const char* fontFilename = "font/FFArrows.ttf";
static constexpr float buttonSize = 48.f;
static constexpr float buttonFontSize = 40.f;
static constexpr Color buttonColorNav{0x96afff};
static constexpr Color buttonColorQuit{0x606060};
static constexpr auto buttonHighlightTime = 300ms;
static constexpr auto buttonHighlightAlpha = .5f;

HelpScreen::HelpScreen(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_demux(),
    m_theora(),
    m_imgY(BufferImage::create(instance, {640, 480}, TextureType::channelY, nullptr)),
    m_imgCb(BufferImage::create(instance, {320, 240}, TextureType::channelCb, nullptr)),
    m_imgCr(BufferImage::create(instance, {320, 240}, TextureType::channelCr, nullptr)),
    m_imgTime(-1.f),
    m_startTime(0.f),
    m_index(0),
    m_hint(instance, "", true),
    m_buttonFont(decoders::ttf(instance, fontFilename)),
    m_buttons({
        Button{TextImage::create(instance, *m_buttonFont, ">"), Key::right, buttonColorNav, true, {}, {}},
        Button{TextImage::create(instance, *m_buttonFont, "<"), Key::left, buttonColorNav, false, {}, {}},
        Button{TextImage::create(instance, *m_buttonFont, "Q"), Key::exit, buttonColorQuit, true, {}, {}}
    })
{
    loadPart(0);
}

void HelpScreen::loadPart(unsigned index) {
    Log::debug<Log::video>("New video: ", files[index]);
    m_demux.reset();
    m_demux.emplace(m_instance.files().system(files[index])->read(), 1);
    m_theora.emplace(m_demux.value());
    auto& info = m_theora.value().info();
    if(info.pic_width != 640 || info.pic_height != 480)
        Log::fatal("Video expected in 640x480!");
    if(info.pixel_fmt != TH_PF_420)
        Log::fatal("Video expected in 4:2:0 Y'CbCr!");
    m_vBuffer.clear();
    m_startTime = timeAlive();
    fill_buffers();
    auto lang = m_instance.persist().get("subtitles", "cs"s);
    m_hint.setText(lang == "cs" ? help_cs[index] : help_en[index]);
}

void HelpScreen::loopVideo() {
    Log::debug<Log::video>("video loop");
    m_demux.value().reset();
    m_theora.value().reset();
    m_vBuffer.clear();
    m_startTime = timeAlive();
    fill_buffers();
}

float HelpScreen::timeSinceStart() {
    return timeAlive() - m_startTime;
}

void HelpScreen::fill_buffers() {
    if(!m_vBuffer.empty() && !m_theora.value().done() && m_vBuffer.back().time < timeSinceStart()) {
        Log::warn("Video lagging by ", timeSinceStart() - m_vBuffer.back().time, "s, skipping to next keyframe, ",
            "dropping ", m_vBuffer.size(), " decoded frames");
        m_vBuffer.clear();
        m_theora.value().skipToKey();
    }

    while(m_vBuffer.size() < vBufSize) {
        m_vBuffer.emplace_back();
        auto& frame = m_vBuffer.back();
        if(!(m_theora.value() >> frame)) {
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
    if(m_vBuffer.size() == 1 && m_vBuffer.front().time < timeSinceStart() && m_theora.value().done())
        loopVideo();
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
    m_hint.draw(target);

    for(auto& button : m_buttons) {
        if(!button.visible)
            continue;
        float alpha = 1.f;
        auto time = liveTime();
        if(time < button.fadeTime)
            alpha += (button.fadeTime - time) / buttonHighlightTime * buttonHighlightAlpha;
        const auto buttonProgram = m_instance.graphics().shaders().button({ .color = button.color.gl(alpha) });
        constexpr auto extent = FCoords{buttonSize, buttonSize};
        target.draw(button.image, buttonProgram, coords, { .dest = button.pos - extent / 2.f, .area = extent });
    };
}

bool HelpScreen::own_pointer(FCoords coords) {
    constexpr auto extent = FCoords{buttonSize, buttonSize};
    for(auto& button : m_buttons)
        if(button.visible && (coords - button.pos).within(-extent / 2.f, extent / 2.f)) {
            own_key(button.key);
            button.fadeTime = liveTime() + buttonHighlightTime;
            return true;
        }
    return false;
}

bool HelpScreen::own_key(Key key) {
    switch(key) {
        case Key::right:
        case Key::space:
            advance(+1);
            return true;
        case Key::left:
            advance(-1);
            return true;
        case Key::exit:
            exit();
            return true;
        default:
            return false;
    }
}

void HelpScreen::advance(int diff) {
    if(m_index == 0 && diff == -1)
        return;
    m_index += diff;
    if(m_index == files.size())
        exit();
    else
        loadPart(m_index);
    m_buttons[1].visible = m_index > 0;
    m_buttons[2].visible = m_index == 0;
}

void HelpScreen::exit() {
    m_instance.screens().startMode(ScreenManager::Mode::WorldMap);
}

void HelpScreen::own_resize() {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    m_buttonFont->setSizes(buttonFontSize, 0.f, coords.scale);

    const auto& nullCoords = m_instance.graphics().coords(Graphics::CoordSystems::null);
    FCoords cREdge{nullCoords.size.x, nullCoords.size.y / 2.f};
    m_buttons[0].pos = coords.out2in(cREdge) - FCoords{buttonSize, 0.f};
    FCoords cLEdge{0.f, nullCoords.size.y / 2.f};
    m_buttons[1].pos = coords.out2in(cLEdge) + FCoords{buttonSize, 0.f};
    m_buttons[2].pos = m_buttons[1].pos;
}
