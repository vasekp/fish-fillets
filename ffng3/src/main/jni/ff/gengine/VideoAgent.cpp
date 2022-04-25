/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "VideoAgent.h"

#include "File.h"
#include "ImgException.h"
#include "AgentPack.h"
#include "SimpleMsg.h"
#include "StringMsg.h"
#include "OptionAgent.h"
#include "SysVideo.h"
#include "FFNGVideo.h"
#include "FFNGApp.h"

//-----------------------------------------------------------------
/**
 * Init SDL and grafic window.
 * @throws SDLException if there is no usuable video mode
 */
    void
VideoAgent::own_init()
{
    m_screen = NULL;

    initVideoMode();
}
//-----------------------------------------------------------------
/**
 * Draw all drawer from list.
 * First will be drawed first.
 */
    void
VideoAgent::own_update()
{
    drawOn(m_screen);
    FFNGVideo::flip(m_screen);
}
//-----------------------------------------------------------------
/**
 * Shutdown SDL.
 */
    void
VideoAgent::own_shutdown()
{
        //TODO
    //FFNG no quit for android... SDL_Quit();
}

//-----------------------------------------------------------------
/**
 * Init video mode along options.
 * Change window only when necessary.
 *
 * @throws SDLException when video mode cannot be made,
 * the old video mode remain usable
 */
    void
VideoAgent::initVideoMode()
{
    OptionAgent *options = OptionAgent::agent();
    int screen_width = options->getAsInt("screen_width", 640);
    int screen_height = options->getAsInt("screen_height", 480);

    SysVideo::setCaption(options->getParam("caption", "A game"));
    if (NULL == m_screen
            || m_screen->getWidth() != screen_width
            || m_screen->getHeight() != screen_height)
    {
    	/*FFNG*/FFNGApp::setGameState(options->getAsInt("game_state", FFNGApp::GAMESTATE_LOADING));
        changeVideoMode(screen_width, screen_height);
    }
}
//-----------------------------------------------------------------
/**
 * Init new video mode.
 * NOTE: m_screen pointer will change
 */
    void
VideoAgent::changeVideoMode(int screen_width, int screen_height)
{
    OptionAgent *options = OptionAgent::agent();
    int screen_bpp = options->getAsInt("screen_bpp", 32);
    int videoFlags = getVideoFlags();

    //TODO: check VideoModeOK and available ListModes
    /* FFNG */ if(m_screen) { delete m_screen; m_screen = NULL; }
    SDL_Surface *newScreen =
        FFNGVideo::setVideoMode(screen_width, screen_height, screen_bpp, videoFlags);

    if (newScreen) {
        m_screen = newScreen;
    }
    else {
        throw std::runtime_error("setVideoMode returned null");
    }
}
//-----------------------------------------------------------------
/**
 * Obtain video information about best video mode.
 * @return best video flags
 */
    int
VideoAgent::getVideoFlags()
{
    int videoFlags  = 0;

    return videoFlags;
}
//-----------------------------------------------------------------
/**
 * Handle incoming message.
 */
    void
VideoAgent::receiveSimple(const SimpleMsg *msg)
{
    Log::warn("unknown msg %s", msg->toString().c_str());
}
//-----------------------------------------------------------------
/**
 * Handle incoming message.
 * @throws UnknownMsgException
 */
    void
VideoAgent::receiveString(const StringMsg *msg)
{
    Log::warn("unknown msg %s", msg->toString().c_str());
}

