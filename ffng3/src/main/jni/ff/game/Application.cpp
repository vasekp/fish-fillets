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

#include <android/log.h>

//FFNG #include "SDL.h"
#include <stdio.h> // for fflush, stdout

#include "FFNGApp.h"

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
Application::init(int argc, char *argv[])
{
    MessagerAgent::agent()->addListener(this);
    m_agents->init(Name::VIDEO_NAME);
    prepareOptions(argc, argv);
    customizeGame();

    m_agents->init(Name::TIMER_NAME);
    addSoundAgent();

    m_agents->init();
}
//-----------------------------------------------------------------
    void
Application::run()
{
    int cycles = 0;
    while (!m_quit) {
        m_agents->update();
        if (++cycles % 100 == 0)
        	__android_log_print(ANDROID_LOG_DEBUG, "FFNG", "cycles: %d", cycles);
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
//-----------------------------------------------------------------
    void
Application::prepareOptions(int argc, char *argv[])
{
    OptionParams params;
    params.addParam("systemdir", OptionParams::TYPE_PATH,
            "File to game data");
    params.addParam("userdir", OptionParams::TYPE_PATH,
            "File to game data");
    params.addParam("lang", OptionParams::TYPE_STRING,
            "2-letter code (e.g., en, cs, fr, de)");
    params.addParam("speech", OptionParams::TYPE_STRING,
            "Lang for speech");
    params.addParam("subtitles", OptionParams::TYPE_BOOLEAN,
            "Enable subtitles");
    params.addParam("fullscreen", OptionParams::TYPE_BOOLEAN,
            "Turn fullscreen on/off");
    params.addParam("show_steps", OptionParams::TYPE_BOOLEAN,
            "Show a step counter in levels");
    params.addParam("sound", OptionParams::TYPE_BOOLEAN,
            "Turn sound on/off");
    params.addParam("volume_sound", OptionParams::TYPE_NUMBER,
            "Sound volume in percentage");
    params.addParam("volume_music", OptionParams::TYPE_NUMBER,
            "Music volume in percentage");
    params.addParam("worldmap", OptionParams::TYPE_STRING,
            "File to the worldmap file");
    params.addParam("cache_images", OptionParams::TYPE_BOOLEAN,
            "Cache images (default=true)");
    params.addParam("sound_frequency", OptionParams::TYPE_NUMBER,
            "Sound sample rate (default=44100)");
    params.addParam("strict_rules", OptionParams::TYPE_BOOLEAN,
                    "Disallow pushing of partially supported objects (default=true)");
    params.addParam("replay_level", OptionParams::TYPE_STRING,
                    "Replay the solution for the given level codename");
    OptionAgent::agent()->parseCmdOpt(argc, argv, params);
}
//-----------------------------------------------------------------
/**
 * Run init script.
 * @throws ResourceException when data are not available
 */
    void
Application::customizeGame()
{
    File initfile = File::internal("script/init.lua");
    if (initfile.exists()) {
        ScriptAgent::agent()->scriptInclude(initfile);
    }
    else {
        throw ResourceException(ExInfo("init file not found")
                .addInfo("path", initfile.getPath())
                .addInfo("systemdir",
                    OptionAgent::agent()->getParam("systemdir"))
                .addInfo("userdir",
                    OptionAgent::agent()->getParam("userdir"))
                .addInfo("hint",
                    "try command line option \"systemdir=path/to/data\""));
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

