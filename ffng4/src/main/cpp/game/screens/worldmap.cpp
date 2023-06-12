#include "worldmap.h"
#include "screenmanager.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_staticFrame(Frames::none)
{
    m_music = m_instance.audio().loadMusic("music/menu.ogg");
    addImage("images/menu/map.png", "background");
    addImage("images/menu/map_mask.png", "mask");
    addImage("images/menu/map_lower.png", "masked");
    addImage("images/menu/loading.png", "loading");
    for(int i = 0; i < 5; i++) {
        auto name = "images/menu/n"s + (char)('0' + i) + ".png";
        m_nodeImages.push_back(addImage(name));
    }

    m_maskColors.insert({Frames::exit, MaskColors::exit});
    m_maskColors.insert({Frames::options, MaskColors::options});
    m_maskColors.insert({Frames::intro, MaskColors::intro});
    m_maskColors.insert({Frames::credits, MaskColors::credits});

    for(const auto& [name, record] : m_instance.levels()) {
        if(record.maskColor != LevelRecord::noColor)
            m_forks.push_back(&record);
        if(record.state() == LevelState::open)
            m_open.push_back(&record);
    }
}

void WorldMap::staticFrame(Frames frame) {
    m_staticFrame = frame;
    m_instance.screens().drawFrame();
}

void WorldMap::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
    m_instance.screens().announceLevel("");
}

void WorldMap::own_draw(const DrawTarget& target, float dt) {
    const auto& copyProgram = m_instance.graphics().shaders().copy;
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);

    m_instance.graphics().setMask(getImage("mask"));
    target.blit(getImage("background"), coords, copyProgram);
    if(m_staticFrame != Frames::loading && !m_pm) {
        drawMasked(target, MaskColors::mainBranch);
        for(const auto& record : m_forks)
            if(record->solved)
                drawMasked(target, record->maskColor);
        for(const auto& [name, record] : m_instance.levels())
            if(record.visible() && record.solved)
                target.blit(m_nodeImages[0], coords, copyProgram, record.coords.fx() - nodeRadius, record.coords.fy() - nodeRadius);
        float phase = std::fmod(timeAlive(), 10.f);
        float sin2 = 3.f * std::pow(std::sin((float)M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        const auto& alphaProgram = m_instance.graphics().shaders().alpha;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), sin2 - (float)base);
        for(const auto& record : m_open) {
            if(!record->visible())
                continue;
            target.blit(m_nodeImages[base + 1], coords, copyProgram, record->coords.fx() - nodeRadius, record->coords.fy() - nodeRadius);
            target.blit(m_nodeImages[base + 2], coords, alphaProgram, record->coords.fx() - nodeRadius, record->coords.fy() - nodeRadius);
        }
    }

    if(auto hover = m_instance.inputSourceMasked().hover(); hover != IInputSource::noHover) {
        auto hcoords = m_instance.graphics().coords(Graphics::CoordSystems::base).out2in(hover);
        for(const auto& area : areas) {
            if(hcoords.within(area.from, area.to))
                drawMasked(target, m_maskColors.at(area.frame));
        }
    }

    if(m_instance.screens().options().visible())
        drawMasked(target, m_maskColors.at(Frames::options));

    if(m_pm && m_staticFrame != Frames::loading)
        m_pm->draw(target, dt);

    switch(m_staticFrame) {
        case Frames::loading:
            target.blit(getImage("loading"), coords, copyProgram, 227, 160);
            break;
        case Frames::exit:
        case Frames::intro:
        case Frames::credits:
            drawMasked(target, m_maskColors.at(m_staticFrame));
            break;
        default:
            break;
    }
}

bool WorldMap::own_pointer(FCoords coords, bool longPress) {
    for(const auto& area : areas)
        if(coords.within(area.from, area.to)) {
            switch(area.frame) {
                case Frames::exit:
                    staticFrame(WorldMap::Frames::exit);
                    m_instance.quit();
                    return true;
                case Frames::options:
                    m_instance.screens().options().show();
                    return true;
                case Frames::intro:
                    staticFrame(WorldMap::Frames::intro);
                    m_instance.screens().startMode(ScreenManager::Mode::Intro);
                    return true;
                case Frames::credits:
                    staticFrame(WorldMap::Frames::credits);
                    m_instance.screens().startMode(ScreenManager::Mode::Credits);
                    return true;
                default:;
            }
        }
    if(m_pm) {
        switch(auto button = m_pm->findButton(coords)) {
            case Pedometer::Buttons::retry:
            case Pedometer::Buttons::replay: {
                staticFrame(WorldMap::Frames::loading);
                auto& level = m_instance.screens().startLevel(m_pm->record());
                if(button == Pedometer::Buttons::replay)
                    level.replay();
                return true;
            }
            case Pedometer::Buttons::close:
                m_pm.reset();
                m_instance.screens().announceLevel("");
                return true;
            default:
                return false;
        }
    }
    /* else */
    auto it = std::find_if(m_instance.levels().begin(), m_instance.levels().end(), [coords](auto& pair) {
        return pair.second.coords && length(coords - pair.second.coords) < nodeTolerance;
    });
    if(it != m_instance.levels().end()) {
        const auto& record = it->second;
        if(record.state() == LevelState::locked)
            return false;
        m_instance.screens().announceLevel(it->second.description.at("cs")); // TODO
        if(record.state() == LevelState::solved)
            m_pm.emplace(m_instance, it->second);
        else {
            staticFrame(WorldMap::Frames::loading);
            m_instance.screens().startLevel(it->second);
        }
        return true;
    } else
        return false;
}

bool WorldMap::own_key(Key key) {
    if(key == Key::exit) {
        if(m_pm) {
            m_pm.reset();
            m_instance.screens().announceLevel("");
        } else {
            staticFrame(WorldMap::Frames::exit);
            m_instance.quit();
        }
        return true;
    } else
        return false;
}

void WorldMap::drawMasked(const DrawTarget& target, Color maskColor) {
    const auto& maskProgram = m_instance.graphics().shaders().maskCopy;
    glUseProgram(maskProgram);
    glUniform4fv(maskProgram.uniform("uMaskColor"), 1, maskColor.gl().data());
    target.blit(getImage("masked"), m_instance.graphics().coords(Graphics::CoordSystems::base), maskProgram);
}

void WorldMap::own_resume() {
    m_staticFrame = Frames::none;
}
