/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "GameState.h"

#include "StateManager.h"
#include "InputHandler.h"
#include "MultiDrawer.h"

#include "InputAgent.h"
#include "VideoAgent.h"
#include "MessagerAgent.h"
#include "SimpleMsg.h"

//-----------------------------------------------------------------
GameState::GameState()
{
    m_nextState = NULL;
    m_handler = NULL;
    m_active = false;
    m_onBg = false;
    m_drawer = new MultiDrawer();
}
//-----------------------------------------------------------------
GameState::~GameState()
{
    if (m_handler) {
        delete m_handler;
    }
    delete m_drawer;
}
//-----------------------------------------------------------------
/**
 * Obtain input handler.
 * @param new_handler new input handler
 */
    void
GameState::takeHandler(InputHandler *new_handler)
{
    if (m_handler) {
        delete m_handler;
    }
    m_handler = new_handler;
}
//-----------------------------------------------------------------
/**
 * Returns wrapped input.
 */
    const InputProvider *
GameState::getInput()
{
    return m_handler;
}
//-----------------------------------------------------------------
    void
GameState::initState(StateManager *manager)
{
//    LOG_DEBUG(ExInfo("initState").addInfo("name", getName()));
    MessagerAgent::agent()->addListener(this);
    m_manager = manager;
    m_active = true;
    m_onBg = false;
    own_initState();
}
//-----------------------------------------------------------------
/**
 * @throws LogicException when state is not active
 */
    void
GameState::updateState()
{
    if (!m_active) {
        throw std::logic_error("update - state is not active");
    }

    own_updateState();
}
//-----------------------------------------------------------------
/**
 * @throws LogicException when state is not active
 */
    void
GameState::pauseState()
{
    if (!m_active) {
        throw std::logic_error("pause - state is not active");
    }

    own_pauseState();
    m_active = false;
    m_onBg = false;
}
//-----------------------------------------------------------------
/**
 * Reactivate state after pause.
 * @throws LogicException when state is already active
 */
    void
GameState::resumeState()
{
    if (m_active) {
        throw std::logic_error("resume - state is already active");
    }
    m_active = true;
    own_resumeState();
}
//-----------------------------------------------------------------
/**
 * Clean state after run.
 * @throws LogicException when state is not active
 */
    void
GameState::cleanState()
{
    Log::debug("cleanState: %s", getName());
    if (!m_active) {
        throw std::logic_error("clean - state is not active");
    }
    own_cleanState();
    unHandlers();

    m_active = false;
    m_onBg = false;
    m_manager = NULL;
    removeWatchers();
    MessagerAgent::agent()->removeListener(getName());
}
//-----------------------------------------------------------------
    void
GameState::quitState()
{
    if (m_nextState) {
        changeState(m_nextState);
    }
    else {
        m_manager->popState(this);
    }
}
//-----------------------------------------------------------------
    void
GameState::pushState(GameState *new_state)
{
    m_manager->pushState(this, new_state);
}
//-----------------------------------------------------------------
    void
GameState::changeState(GameState *new_state)
{
    m_manager->changeState(this, new_state);
}
//-----------------------------------------------------------------
    void
GameState::noteBg()
{
//    LOG_DEBUG(ExInfo("noteBg").addInfo("name", getName()));
    own_noteBg();
    m_onBg = true;
}
//-----------------------------------------------------------------
    void
GameState::noteFg()
{
//    LOG_DEBUG(ExInfo("noteFg").addInfo("name", getName()));
    m_onBg = false;
    own_noteFg();
}
//-----------------------------------------------------------------
/**
 * Install own video and input handler.
 */
    void
GameState::installHandlers()
{
//    LOG_DEBUG(ExInfo("installHandlers").addInfo("state", getName()));
    InputAgent::agent()->installHandler(m_handler);
    VideoAgent::agent()->acceptDrawer(m_drawer);
}
//-----------------------------------------------------------------
/**
 * Uninstall own video and input handler.
 */
    void
GameState::unHandlers()
{
    InputAgent::agent()->installHandler(NULL);
    VideoAgent::agent()->removeDrawer(m_drawer);
}

//-----------------------------------------------------------------
/**
 * Add new drawable to drawers fifo.
 * NOTE: order is important,
 * the first inserted drawer will be on background
 */
    void
GameState::registerDrawable(Drawable *drawable)
{
    m_drawer->acceptDrawer(drawable);
}
//-----------------------------------------------------------------
    void
GameState::deregisterDrawable(const Drawable *drawable)
{
    m_drawer->removeDrawer(drawable);
}
//-----------------------------------------------------------------
/**
 * Handle incoming message.
 * Messages:
 * - quit ... quit state
 */
    void
GameState::receiveSimple(const SimpleMsg *msg)
{
    if (msg->equalsName("quit")) {
        quitState();
    }
    else {
        Log::warn("unknown msg %s", msg->toString().c_str());
    }
}


