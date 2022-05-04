#include "worldmap.h"
#include "statemanager.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_nextFrame(Frames::none)
{
    setBackground("orig/map.png");
    setMusic("music/menu.ogg");
    addImage("orig/loading.png", "loading");
    addImage("orig/map-mask.png", "mask");
    addImage("images/menu/map_lower.png", "masked");
    for(int i = 0; i < 5; i++) {
        auto name = "images/menu/n"s + (char)('0' + i) + ".png";
        nodeImages.push_back(addImage(name));
    }

    maskColors.insert({Frames::exit, MaskColors::exit});
    maskColors.insert({Frames::options, MaskColors::options});
    maskColors.insert({Frames::intro, MaskColors::intro});
    maskColors.insert({Frames::credits, MaskColors::credits});
}

void WorldMap::staticFrame(Frames frame) {
    m_nextFrame = frame;
    m_instance.graphics().drawFrame();
}

void WorldMap::own_load() {
    m_instance.graphics().readBuffer().setImage(getImage("mask"));
    m_instance.graphics().setMask(getImage("mask"));
}

void WorldMap::own_draw() {
    const auto& canvas = m_instance.graphics().canvas();
    const auto& copyProgram = m_instance.graphics().shaders().copy;

    canvas.drawImage(getImage("background"), copyProgram);
    if(m_nextFrame != Frames::loading) {
        drawMasked(MaskColors::mainBranch);
        float phase = std::fmod(timeSinceLoad(), 10.f);
        float sin2 = 3.f * std::powf(std::sinf(M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        canvas.drawImage(*nodeImages[base + 1], copyProgram, 320 - nodeRadius, 121 - nodeRadius);
        const auto& alphaProgram = m_instance.graphics().shaders().alpha;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), sin2 - (float)base);
        canvas.drawImage(*nodeImages[base + 2], alphaProgram, 320 - nodeRadius, 121 - nodeRadius);
    }

    switch(m_nextFrame) {
        case Frames::loading:
            canvas.drawImage(getImage("loading"), copyProgram, 227, 160);
            break;
        case Frames::exit:
        case Frames::options:
        case Frames::intro:
        case Frames::credits:
            drawMasked(maskColors.at(m_nextFrame));
            break;
        default:
            break;
    }
    m_nextFrame = Frames::none;
}

bool WorldMap::own_mouse(unsigned int x, unsigned int y) {
    auto mask_color = m_instance.graphics().readBuffer().getPixel(x, y);
    if(mask_color == WorldMap::MaskColors::exit) {
        staticFrame(WorldMap::Frames::exit);
        m_instance.quit();
    } else if(mask_color == WorldMap::MaskColors::options) {
        staticFrame(WorldMap::Frames::options);
    } else if(mask_color == WorldMap::MaskColors::intro) {
        staticFrame(WorldMap::Frames::intro);
        m_instance.live = false;
        m_instance.states().setState(GameState::Intro);
    } else if(mask_color == WorldMap::MaskColors::credits) {
        staticFrame(WorldMap::Frames::credits);
    } else {
        staticFrame(WorldMap::Frames::loading);
        m_instance.states().setState(GameState::TestScreen);
    }
    return true;
}

void WorldMap::drawMasked(Color c) {
    const auto& maskProgram = m_instance.graphics().shaders().maskCopy;
    glUseProgram(maskProgram);
    glUniform4fv(maskProgram.uniform("uMaskColor"), 1, c.gl().get());
    m_instance.graphics().canvas().drawImage(getImage("masked"), maskProgram);
}