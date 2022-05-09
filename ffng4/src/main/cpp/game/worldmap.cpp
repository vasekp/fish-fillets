#include "worldmap.h"
#include "statemanager.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_nextFrame(Frames::none)
{
    m_music = m_instance.audio().loadMusic("music/menu.ogg");
    addImage("override/map.png", "background");
    addImage("override/loading.png", "loading");
    addImage("override/map-mask.png", "mask");
    addImage("images/menu/map_lower.png", "masked");
    for(int i = 0; i < 5; i++) {
        auto name = "images/menu/n"s + (char)('0' + i) + ".png";
        m_nodeImages.push_back(addImage(name));
    }

    m_maskColors.insert({Frames::exit, MaskColors::exit});
    m_maskColors.insert({Frames::options, MaskColors::options});
    m_maskColors.insert({Frames::intro, MaskColors::intro});
    m_maskColors.insert({Frames::credits, MaskColors::credits});

    for(const auto& [name, record] : m_instance.levels()) {
        if(record->maskColor != LevelRecord::no_color)
            m_forks.push_back(record);
    }
}

void WorldMap::staticFrame(Frames frame) {
    m_nextFrame = frame;
    m_instance.graphics().drawFrame();
}

void WorldMap::own_start() {
    m_music.rewind();

    m_open.clear();
    for(const auto& [name, record] : m_instance.levels()) {
        if(record->state() == LevelState::open)
            m_open.push_back(record);
    }

    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

void WorldMap::own_refresh() {
    m_instance.graphics().readBuffer().setImage(getImage("mask"));
    m_instance.graphics().setMask(getImage("mask"));
}

void WorldMap::own_draw() {
    const auto& canvas = m_instance.graphics().canvas();
    const auto& copyProgram = m_instance.graphics().shaders().copy;

    canvas.drawImage(getImage("background"), copyProgram);
    if(m_nextFrame != Frames::loading) {
        drawMasked(MaskColors::mainBranch);
        for(const auto& record : m_forks)
//            if(record->solved)
                drawMasked(record->maskColor);
        for(const auto& [name, record] : m_instance.levels())
            if(record->solved)
                canvas.drawImage(m_nodeImages[0], copyProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
        float phase = std::fmod(timeAlive(), 10.f);
        float sin2 = 3.f * std::powf(std::sinf(M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        const auto& alphaProgram = m_instance.graphics().shaders().alpha;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), sin2 - (float)base);
        for(const auto& record : m_open) {
            canvas.drawImage(m_nodeImages[base + 1], copyProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
            canvas.drawImage(m_nodeImages[base + 2], alphaProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
        }
    }

    switch(m_nextFrame) {
        case Frames::loading:
            canvas.drawImage(getImage("loading"), copyProgram, 227, 160);
            break;
        case Frames::exit:
        case Frames::options:
        case Frames::intro:
        case Frames::credits:
            drawMasked(m_maskColors.at(m_nextFrame));
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
        m_instance.states().setState(GameState::Credits);
    } else {
        auto fx = (float)x, fy = (float)y;
        auto it = std::find_if(m_instance.levels().begin(), m_instance.levels().end(), [fx, fy](auto& pair) {
            auto& coords = pair.second->coords;
            return std::hypot(coords.fx() - fx, coords.fy() - fy) < nodeTolerance;
        });
        if(it != m_instance.levels().end()) {
            staticFrame(WorldMap::Frames::loading);
            m_instance.states().startLevel(*it->second);
        }
    }
    return true;
}

void WorldMap::drawMasked(Color c) {
    const auto& maskProgram = m_instance.graphics().shaders().maskCopy;
    glUseProgram(maskProgram);
    glUniform4fv(maskProgram.uniform("uMaskColor"), 1, c.gl().get());
    m_instance.graphics().canvas().drawImage(getImage("masked"), maskProgram);
}