#include "worldmap.h"
#include "screenmanager.h"
#include "subsystem/persist.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_staticFrame(Frames::none),
    m_frameShown(false)
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

    m_showEnding = std::all_of(m_instance.levels().begin(), m_instance.levels().end(),
        [](const auto& element) -> bool {
            const auto& record = element.second;
            return !record.final() || record.solved;
        });

    for(const auto& [name, record] : m_instance.levels()) {
        if(record.maskColor != LevelRecord::noColor)
            m_forks.push_back(&record);
        if((!record.ending() || m_showEnding) && record.state() == LevelState::open)
            m_open.push_back(&record);
    }
}

void WorldMap::staticFrame(Frames frame, std::function<void()>&& action) {
    m_staticFrame = frame;
    m_nextAction = std::move(action);
}

void WorldMap::own_start() {
    m_instance.audio().clear();
    m_instance.audio().addSource(m_music);
    m_instance.screens().announceLevel("");
}

void WorldMap::own_update() {
    if(m_nextAction && m_frameShown) {
        m_nextAction();
        m_nextAction = nullptr;
    }
}

void WorldMap::own_draw(DrawTarget& target) {
    const auto copyProgram = m_instance.graphics().shaders().copy();
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);

    m_instance.graphics().setMask(getImage("mask"));
    target.draw(getImage("background"), copyProgram, coords);
    if(m_staticFrame != Frames::loading && !m_pm) {
        for(const auto& record : m_forks)
            if(record->depth == 1 || (record->parent && record->parent->solved))
                drawMasked(target, record->maskColor);
        if(m_showEnding)
            drawMasked(target, MaskColors::ending);
        constexpr FCoords nodeOffset{nodeRadius, nodeRadius};
        for(const auto& [name, record] : m_instance.levels())
            if(record.solved)
                target.draw(m_nodeImages[0], copyProgram, coords, { .dest = record.coords - nodeOffset });
        float phase = std::fmod(timeAlive(), 10.f);
        float sin2 = 3.f * std::pow(std::sin((float)M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        const auto alphaProgram = m_instance.graphics().shaders().alpha({ .alpha = sin2 - (float)base });
        for(const auto& record : m_open) {
            target.draw(m_nodeImages[base + 1], copyProgram, coords, { .dest = record->coords - nodeOffset });
            target.draw(m_nodeImages[base + 2], alphaProgram, coords, { .dest = record->coords - nodeOffset });
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
        m_pm->draw(target, timeAlive());

    switch(m_staticFrame) {
        case Frames::loading:
            target.draw(getImage("loading"), copyProgram, coords, { .dest = FCoords{227, 160} });
            m_frameShown = true;
            break;
        case Frames::exit:
        case Frames::intro:
        case Frames::credits:
            drawMasked(target, m_maskColors.at(m_staticFrame));
            m_frameShown = true;
            break;
        default:
            break;
    }
}

bool WorldMap::own_pointer(FCoords coords) {
    for(const auto& area : areas)
        if(coords.within(area.from, area.to)) {
            switch(area.frame) {
                case Frames::exit:
                    staticFrame(WorldMap::Frames::exit, [this]() { m_instance.quit(); });
                    return true;
                case Frames::options:
                    m_instance.screens().options().show();
                    return true;
                case Frames::intro:
                    staticFrame(WorldMap::Frames::intro, [this]() {
                        m_instance.screens().startMode(ScreenManager::Mode::Intro);
                    });
                    return true;
                case Frames::credits:
                    staticFrame(WorldMap::Frames::credits, [this]() {
                        m_instance.screens().startMode(ScreenManager::Mode::Credits);
                    });
                    return true;
                default:;
            }
        }

    if(m_pm) {
        switch(auto button = m_pm->findButton(coords)) {
            case Pedometer::Buttons::retry:
            case Pedometer::Buttons::replay: {
                staticFrame(WorldMap::Frames::loading, [this, button]() {
                    m_instance.screens().startLevel(m_pm->record(), button == Pedometer::Buttons::replay);
                });
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
        if(record.state() == LevelState::locked || (record.ending() && !m_showEnding))
            return false;
        auto lang = m_instance.persist().get("subtitles", "cs"s);
        if(lang.empty())
            lang = "cs"s;
        m_instance.screens().announceLevel(it->second.title.at(lang));
        if(record.state() == LevelState::solved)
            m_pm.emplace(m_instance, it->second, timeAlive());
        else {
            staticFrame(WorldMap::Frames::loading, [this, it]() {
                m_instance.screens().startLevel(it->second);
            });
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
            staticFrame(WorldMap::Frames::exit, [this]() { m_instance.quit(); });
        }
        return true;
    } else
        return false;
}

void WorldMap::drawMasked(DrawTarget& target, Color maskColor) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto maskProgram = m_instance.graphics().shaders().maskCopy({ .maskColor = maskColor });
    target.draw(getImage("masked"), maskProgram, coords);
}

void WorldMap::own_resume() {
    m_staticFrame = Frames::none;
}
