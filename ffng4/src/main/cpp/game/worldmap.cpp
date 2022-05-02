#include "worldmap.h"

WorldMap::WorldMap(Instance* instance) :
    GameScreen(instance),
    m_nextFrame(Frames::none)
{
    setBackground("orig/map.png");
    setMusic("music/menu.ogg");
    addImage("loading", "orig/loading.png");
    addImage("mask", "orig/map-mask.png");
    addImage("masked", "images/menu/map_lower.png");

    maskColors.insert({Frames::exit, MaskColors::exit});
    maskColors.insert({Frames::options, MaskColors::options});
    maskColors.insert({Frames::intro, MaskColors::intro});
    maskColors.insert({Frames::credits, MaskColors::credits});
}

void WorldMap::own_load() {
    m_instance->graphics->setMask(getImage("mask"));
}

void WorldMap::own_draw() {
    const auto& canvas = m_instance->graphics->canvas();
    const auto& copyProgram = m_instance->graphics->shaders()->copy;
    const auto& maskProgram = m_instance->graphics->shaders()->maskCopy;

    canvas->drawImage(getImage("background"), copyProgram);

    switch(m_nextFrame) {
        case Frames::loading:
            canvas->drawImage(getImage("loading"), copyProgram, 227, 160);
            break;
        case Frames::exit:
        case Frames::options:
        case Frames::intro:
        case Frames::credits:
            glUseProgram(maskProgram);
            glUniform4fv(maskProgram.uniform("uMaskColor"), 1, maskColors.at(m_nextFrame).gl().get());
            canvas->drawImage(getImage("masked"), maskProgram);
            break;
        default:
            break;
    }
    m_nextFrame = Frames::none;
}

void WorldMap::staticFrame(Frames frame) {
    m_nextFrame = frame;
}