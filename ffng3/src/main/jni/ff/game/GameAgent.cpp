/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "GameAgent.h"

#include "StateManager.h"
#include "WorldMap.h"
#include "File.h"
#include "OptionAgent.h"
#include "FFNGApp.h"

//-----------------------------------------------------------------
    void
GameAgent::own_init()
{
    m_manager = new StateManager();
    WorldMap *worldmap = new WorldMap();
    worldmap->initMap(File::internal("script/worldmap.lua"));
    m_manager->pushState(NULL, worldmap);
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
/**
 * Update game.
 * Play level or show menu.
 */
    void
GameAgent::own_update()
{
    m_manager->updateGame();
}
//-----------------------------------------------------------------
/**
 * Remember playtime.
 */
    void
GameAgent::own_shutdown()
{
    OptionAgent *options = OptionAgent::agent();
    int playtime = options->getAsInt("playtime");
    playtime += FFNGApp::getTicks() / 1000;
    options->setPersistent("playtime", playtime);

    delete m_manager;
}
