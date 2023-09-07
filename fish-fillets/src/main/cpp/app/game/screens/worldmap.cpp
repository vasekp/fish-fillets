#include "worldmap.h"
#include "screenmanager.h"
#include "subsystem/persist.h"
#include "subsystem/graphics.h"
#include "subsystem/audio.h"
#include "game/structure/gametree.h"

static constexpr int nodeRadius = 9;
static constexpr int nodeTolerance = 15;

struct MaskColors {
    static constexpr Color help = 0x008080;
    static constexpr Color exit = 0x008080;
    static constexpr Color options = 0x008000;
    static constexpr Color intro = 0x000080;
    static constexpr Color credits = 0x808000;
    static constexpr Color ending = 0x808080;
};

struct ActiveAreas {
    FCoords from;
    FCoords to;
    WorldMap::Frames frame;
};

static constexpr std::array<ActiveAreas, 5> areas {{
    {{0, 0}, {138, 127}, WorldMap::Frames::intro},
#ifdef FISH_FILLETS_DISABLE_EXIT
    {{517, 0}, {640, 130}, WorldMap::Frames::help},
#else
    {{517, 0}, {640, 130}, WorldMap::Frames::exit},
#endif
    {{0, 364}, {124, 480}, WorldMap::Frames::credits},
    {{0, 403}, {145, 480}, WorldMap::Frames::credits}, /* a single rectangle would overlap a level */
    {{487, 362}, {640, 480}, WorldMap::Frames::options}
}};

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_input(instance, *this),
    m_staticFrame(Frames::none),
    m_frameShown(false)
{
    m_music = m_instance.audio().loadMusic("music/menu.ogg");
#ifdef FISH_FILLETS_DISABLE_EXIT
    addImage("images/menu/map_help.png", "background");
    addImage("images/menu/map_help_lower.png", "masked");
#else
    addImage("images/menu/map.png", "background");
    addImage("images/menu/map_lower.png", "masked");
#endif
    addImage("images/menu/map_mask.png", "mask", TextureType::mask);
    addImage("images/menu/loading.png", "loading");
    for(int i = 0; i < 5; i++) {
        auto name = "images/menu/n"s + (char)('0' + i) + ".png";
        m_nodeImages.push_back(addImage(name));
    }

#ifdef FISH_FILLETS_DISABLE_EXIT
    m_maskColors.insert({Frames::help, MaskColors::help});
#else
    m_maskColors.insert({Frames::exit, MaskColors::exit});
#endif
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
    m_instance.audio().addSource(m_music);
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
        float sin2 = 3.f * std::pow(std::sin(std::numbers::pi_v<float> * phase), 2.f);
        float base = std::min(std::floor(sin2), 2.f);
        const auto alphaProgram = m_instance.graphics().shaders().alpha({ .alpha = sin2 - base });
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
        m_pm->draw(target, liveTime());

    switch(m_staticFrame) {
        case Frames::loading:
            target.draw(getImage("loading"), copyProgram, coords, { .dest = FCoords{227, 160} });
            m_frameShown = true;
            break;
        case Frames::exit:
        case Frames::help:
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
                case Frames::help:
                    staticFrame(WorldMap::Frames::help, [this]() {
                        m_instance.screens().startMode(ScreenManager::Mode::Help);
                    });
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
        auto lang = m_instance.persist().get("subtitles", ""s);
        if(lang.empty())
            lang = "cs";
        m_instance.screens().announceLevel(it->second.title.at(lang));
        if(record.state() == LevelState::solved)
            m_pm.emplace(m_instance, it->second, liveTime());
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
            m_instance.quit();
        }
        return true;
    } else
        return false;
}

void WorldMap::drawMasked(DrawTarget& target, Color maskColor) {
    const auto& coords = m_instance.graphics().coords(Graphics::CoordSystems::base);
    const auto maskProgram = m_instance.graphics().shaders().maskCopy({ .maskColor = maskColor.gl() });
    target.draw({
        getImage("masked")->texture(),
        getImage("mask")->texture()
    }, maskProgram, coords);
}

void WorldMap::own_resume() {
    m_staticFrame = Frames::none;
}
