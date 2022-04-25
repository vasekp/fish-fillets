/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "Application.h"

#include "Log.h"
#include "File.h"
#include "Random.h"
#include "AgentPack.h"
#include "MessagerAgent.h"
#include "VideoAgent.h"
#include "InputAgent.h"
#include "TimerAgent.h"
#include "GameAgent.h"
#include "SoundAgent.h"
#include "SDLSoundAgent.h"
#include "DummySoundAgent.h"
#include "ScriptAgent.h"
#include "OptionAgent.h"
#include "SubTitleAgent.h"
#include "ResourceException.h"
#include "OptionParams.h"
#include "Font.h"

#include "SimpleMsg.h"
#include "StringMsg.h"

#include "FFNGApp.h"
#include <stdexcept>
#include <string>

using namespace std::string_literals;

//-----------------------------------------------------------------
Application::Application()
{
    m_quit = false;
    Random::init();
    Font::init();

    m_agents = new AgentPack();
    //NOTE: MessagerAgent is added by AgentPack
    //NOTE: creating order is not significant, names are significant,
    // like rc.d scripts
    m_agents->addAgent(new ScriptAgent());
    m_agents->addAgent(new OptionAgent());
    m_agents->addAgent(new VideoAgent());

    m_agents->addAgent(new InputAgent());

    m_agents->addAgent(new SubTitleAgent());
    m_agents->addAgent(new GameAgent());

    m_agents->addAgent(new TimerAgent());
}
//-----------------------------------------------------------------
Application::~Application()
{
    delete m_agents;
    Font::shutdown();
}
//-----------------------------------------------------------------
    void
Application::init()
{
    MessagerAgent::agent()->addListener(this);
    m_agents->init(Name::VIDEO_NAME);
    loadInitScript();

    m_agents->init(Name::TIMER_NAME);
    addSoundAgent();

    m_agents->init();
}
//-----------------------------------------------------------------
    void
Application::run()
{
    while (!m_quit) {
        m_agents->update();
        m_quit |= FFNGApp::pauseAndDisposeChance();
    }
}
//-----------------------------------------------------------------
    void
Application::shutdown()
{
    m_agents->shutdown();
}
//-----------------------------------------------------------------
/**
 * Run init script.
 * @throws ResourceException when data are not available
 */
    void
Application::loadInitScript()
{
    File initfile = File::internal("script/init.lua");
    if (initfile.exists()) {
        ScriptAgent::agent()->scriptInclude(initfile);
    }
    else {
        throw std::runtime_error("init file not found: "s + initfile.getPath());
    }
}
//-----------------------------------------------------------------
/**
 * Choose SDL or Dummy sound agent.
 * Reads 'sound' config option.
 */
    void
Application::addSoundAgent()
{
    //TODO: better setting sound on/off
    //TODO: move to the SoundAgent
    SoundAgent *soundAgent = NULL;
    if (OptionAgent::agent()->getAsBool("sound", true)) {
        soundAgent = new SDLSoundAgent();
        try {
            soundAgent->init();
        }
        catch (BaseException &e) {
            Log::warn("%s", e.info().info().c_str());
            delete soundAgent;
            soundAgent = new DummySoundAgent();
        }
    }
    else {
        soundAgent = new DummySoundAgent();
    }
    m_agents->addAgent(soundAgent);
}

//-----------------------------------------------------------------
/**
 * Handle incoming message.
 * Messages:
 * - quit ... application quit
 */
    void
Application::receiveSimple(const SimpleMsg *msg)
{
    if (msg->equalsName("quit")) {
        m_quit = true;
    }
    else {
        Log::warn("unknown msg %s", msg->toString().c_str());
    }
}

void
Application::receiveString(const StringMsg *msg)
{
    Log::warn("unknown msg %s", msg->toString().c_str());
}

