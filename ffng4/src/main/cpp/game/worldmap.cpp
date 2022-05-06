#include "worldmap.h"
#include "statemanager.h"

WorldMap::WorldMap(Instance& instance) :
    GameScreen(instance),
    m_nextFrame(Frames::none)
{
    setBackground("override/map.png");
    setMusic("music/menu.ogg");
    addImage("override/loading.png", "loading");
    addImage("override/map-mask.png", "mask");
    addImage("images/menu/map_lower.png", "masked");
    for(int i = 0; i < 5; i++) {
        auto name = "images/menu/n"s + (char)('0' + i) + ".png";
        nodeImages.push_back(addImage(name));
    }

    m_maskColors.insert({Frames::exit, MaskColors::exit});
    m_maskColors.insert({Frames::options, MaskColors::options});
    m_maskColors.insert({Frames::intro, MaskColors::intro});
    m_maskColors.insert({Frames::credits, MaskColors::credits});

    m_script.registerFn("file_include", file_include);
    m_script.registerFn("branch_addNode", branch_addNode);
    m_script.registerFn("branch_setEnding", branch_setEnding);
    m_script.registerFn("worldmap_addDesc", worldmap_addDesc);
    m_script.registerFn("node_bestSolution", node_bestSolution);
    m_script.loadFile("override/worldmap.lua");
}

void WorldMap::staticFrame(Frames frame) {
    m_nextFrame = frame;
    m_instance.graphics().drawFrame();
}

void WorldMap::own_load() {
    m_instance.graphics().readBuffer().setImage(getImage("mask"));
    m_instance.graphics().setMask(getImage("mask"));

    m_open.clear();
    for(const auto& [name, info] : m_levels) {
        if(info.solved)
            continue;
        if(info.parent.empty())
            m_open.push_back(info);
        if(m_levels[info.parent].solved)
            m_open.push_back(info);
    }
}

void WorldMap::own_draw() {
    const auto& canvas = m_instance.graphics().canvas();
    const auto& copyProgram = m_instance.graphics().shaders().copy;

    canvas.drawImage(getImage("background"), copyProgram);
    if(m_nextFrame != Frames::loading) {
        drawMasked(MaskColors::mainBranch);
        for(const auto& branch : m_branches)
            if(m_levels[branch.start].solved)
                drawMasked(branch.maskColor);
        for(const auto& [name, info] : m_levels)
            if(info.solved)
                canvas.drawImage(nodeImages[0], copyProgram, info.coords.x() - nodeRadius, info.coords.y() - nodeRadius);
        float phase = std::fmod(timeSinceLoad(), 10.f);
        float sin2 = 3.f * std::powf(std::sinf(M_PI * phase), 2.f);
        auto base = std::min((int)sin2, 2);
        const auto& alphaProgram = m_instance.graphics().shaders().alpha;
        glUseProgram(alphaProgram);
        glUniform1f(alphaProgram.uniform("uAlpha"), sin2 - (float)base);
        for(const auto& info : m_open) {
            canvas.drawImage(nodeImages[base + 1], copyProgram, info.coords.x() - nodeRadius, info.coords.y() - nodeRadius);
            canvas.drawImage(nodeImages[base + 2], alphaProgram, info.coords.x() - nodeRadius, info.coords.y() - nodeRadius);
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
    } else {
        auto fx = (float)x, fy = (float)y;
        auto it = std::find_if(m_levels.begin(), m_levels.end(), [fx, fy](auto& pair) {
            auto& coords = pair.second.coords;
            return std::hypot(coords.fx() - fx, coords.fy() - fy) < nodeTolerance;
        });
        if(it != m_levels.end()) {
            LOGD("%s", it->first.c_str());
            it->second.solved = true;
        }
        /*staticFrame(WorldMap::Frames::loading);
        m_instance.states().setState(GameState::TestScreen);*/
    }
    return true;
}

void WorldMap::drawMasked(Color c) {
    const auto& maskProgram = m_instance.graphics().shaders().maskCopy;
    glUseProgram(maskProgram);
    glUniform4fv(maskProgram.uniform("uMaskColor"), 1, c.gl().get());
    m_instance.graphics().canvas().drawImage(getImage("masked"), maskProgram);
}

int WorldMap::file_include(lua_State* L) {
    auto [filename] = lua::args<lua::types::string>(L);
    Script::from(L).loadFile(filename);
    return 0;
}

int WorldMap::branch_addNode(lua_State* L) {
    [[maybe_unused]] auto [parent, codename, filename, x, y, color, poster] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::integer,
            lua::types::integer,
            lua::optional<lua::types::integer, -1>,
            lua::optional<lua::types::string>
    >(L);
    auto& screen = dynamic_cast<WorldMap&>(Script::from(L).screen());
    if(!*parent) // Root node
        screen.m_levels[codename] = {filename, "", false, {x, y}};
    else
        screen.m_levels[codename] = {filename, parent, false, {x, y}};
    if(color != -1)
        screen.m_branches.push_back({codename, {(std::uint32_t)color}});
    return 0;
}

int WorldMap::branch_setEnding(lua_State* L) {
    [[maybe_unused]] auto [codename, filename, poster] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    LOGV("branchEnding");
    return 0;
}

int WorldMap::worldmap_addDesc(lua_State* L) {
    [[maybe_unused]] auto [codename, lang, levelname, branch] = lua::args<
            lua::types::string,
            lua::types::string,
            lua::types::string,
            lua::types::string
    >(L);
    LOGV("addDesc");
    return 0;
}

int WorldMap::node_bestSolution(lua_State* L) {
    [[maybe_unused]] auto [codename, moves, name] = lua::args<
            lua::types::string,
            lua::types::integer,
            lua::types::string
    >(L);
    LOGV("bestSolution");
    return 0;
}