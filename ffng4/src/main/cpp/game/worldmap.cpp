#include "worldmap.h"
#include "screens.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_staticFrame(Frames::none)
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
        if(record->state() == LevelState::open)
            m_open.push_back(record);
    }
}

void WorldMap::staticFrame(Frames frame) {
    m_staticFrame = frame;
    m_instance.graphics().drawFrame();
}

void WorldMap::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
}

void WorldMap::own_refresh() {
    m_instance.graphics().readBuffer().setImage(getImage("mask"));
    m_instance.graphics().setMask(getImage("mask"));
    m_text = m_instance.graphics().text("První pokus velice dlouhého textu co asi bude potřeba zalomit");
}

void WorldMap::own_draw(const DrawTarget& target) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;

    target.blit(getImage("background"), copyProgram);
    if(m_staticFrame != Frames::loading) {
        drawMasked(target, MaskColors::mainBranch);
        for(const auto& record : m_forks)
//            if(record->solved)
                drawMasked(target, record->maskColor);
        for(const auto& [name, record] : m_instance.levels())
            if(record->solved)
                target.blit(m_nodeImages[0], copyProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
        float phase = std::fmod(timeAlive(), 10.f);
        float sin2 = 3.f * std::powf(std::sinf(M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        const auto& alphaProgram = m_instance.graphics().shaders().alpha;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), sin2 - (float)base);
        for(const auto& record : m_open) {
            target.blit(m_nodeImages[base + 1], copyProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
            target.blit(m_nodeImages[base + 2], alphaProgram, record->coords.x() - nodeRadius, record->coords.y() - nodeRadius);
        }
    }

    if(m_instance.screens().options())
        drawMasked(target, m_maskColors.at(Frames::options));

    switch(m_staticFrame) {
        case Frames::loading:
            target.blit(getImage("loading"), copyProgram, 227, 160);
            break;
        case Frames::exit:
        case Frames::intro:
        case Frames::credits:
            drawMasked(target, m_maskColors.at(m_staticFrame));
            break;
        default:
            break;
    }

    target.blit(m_text, copyProgram);
}

bool WorldMap::own_mouse(unsigned int x, unsigned int y) {
    auto mask_color = m_instance.graphics().readBuffer().getPixel(x, y);
    if(mask_color == WorldMap::MaskColors::exit) {
        staticFrame(WorldMap::Frames::exit);
        m_instance.quit();
    } else if(mask_color == WorldMap::MaskColors::options) {
        m_instance.screens().options() = !m_instance.screens().options();
    } else if(mask_color == WorldMap::MaskColors::intro) {
        staticFrame(WorldMap::Frames::intro);
        m_instance.screens().startMode(Screens::Mode::Intro);
    } else if(mask_color == WorldMap::MaskColors::credits) {
        staticFrame(WorldMap::Frames::credits);
        m_instance.screens().startMode(Screens::Mode::Credits);
    } else {
        auto fx = (float)x, fy = (float)y;
        auto it = std::find_if(m_instance.levels().begin(), m_instance.levels().end(), [fx, fy](auto& pair) {
            auto& coords = pair.second->coords;
            return std::hypot(coords.fx() - fx, coords.fy() - fy) < nodeTolerance;
        });
        if(it != m_instance.levels().end()) {
            staticFrame(WorldMap::Frames::loading);
            m_instance.screens().startLevel(*it->second);
        }
    }
    return true;
}

void WorldMap::drawMasked(const DrawTarget& target, Color c) {
    const auto& maskProgram = m_instance.graphics().shaders().maskCopy;
    glUseProgram(maskProgram);
    glUniform4fv(maskProgram.uniform("uMaskColor"), 1, c.gl().get());
    target.blit(getImage("masked"), maskProgram);
}

void WorldMap::own_resume() {
    m_staticFrame = Frames::none;
}
