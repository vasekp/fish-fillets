#include "worldmap.h"

WorldMap::WorldMap(Instance* instance) : GameScreen(instance), m_loadingFrame(false) {
    setBackground("orig/map.png");
    setMusic("music/menu.ogg");
    addImage("loading", "orig/loading.png");
}

void WorldMap::own_draw() {
    const auto& canvas = m_instance->graphics->canvas();
    const auto& program = m_instance->graphics->shaders()->copy;

    canvas->drawImage(getImage("background"), program);

    if(m_loadingFrame) {
        canvas->drawImage(getImage("loading"), program, 227, 160);
        m_loadingFrame = false;
    }
}