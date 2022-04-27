/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "OptionAgent.h"

#include "Environ.h"

#include "File.h"
#include "ScriptAgent.h"
#include "StringTool.h"
#include "OptionParams.h"
#include "StringMsg.h"
#include "minmax.h"

#include <stdlib.h> //getenv
#include <string.h> //strlen
#include <locale.h> //setlocale

#ifndef LC_MESSAGES
#define LC_MESSAGES LC_CTYPE
#endif

const char *OptionAgent::CONFIG_FILE = "script/options.lua";
//-----------------------------------------------------------------
/**
 * Set user and sytem dir
 * and process "script/options.lua" - this will set user and system paths
 * and process "script/init.lua".
 */
    void
OptionAgent::own_init()
{
    m_environ = new Environ();
    readUserConfig();
    prepareLang();
}
//-----------------------------------------------------------------
/**
 * Save user config.
 * Delete left messages.
 */
    void
OptionAgent::own_shutdown()
{
    delete m_environ;
}

//-----------------------------------------------------------------
/**
 * Prepare user lang option.
 * For 2-letter lang codes
 * see http://www.w3.org/WAI/ER/IG/ert/iso639.htm
 */
    void
OptionAgent::prepareLang()
{
    setlocale(LC_ALL, "");
    //NOTE: '.' will be decimal point for Lua
    setlocale(LC_NUMERIC, "C");
    if (getParam("lang").empty()) {
        char *form = setlocale(LC_MESSAGES, NULL);
        if (form) {
            int size = min(5, strlen(form));
            if (size >= 2) {
            	setParam("lang", std::string(form, size));
            }
        }
    }
}
//-----------------------------------------------------------------
/**
 * Set param.
 * Notice watchers.
 * When watcher is not available, it will be removed.
 *
 * @param name param name
 * @param value param value
 */
    void
OptionAgent::setParam(const std::string &name, const std::string &value)
{
    m_environ->setParam(name, value);
}
//-----------------------------------------------------------------
/**
 * Store this integer value like string param.
 */
    void
OptionAgent::setParam(const std::string &name, long value)
{
    m_environ->setParam(name, value);
}
//-----------------------------------------------------------------
/**
 * Return value.
 * Implicit value is "".
 *
 * @param name param name
 * @param implicit default value = ""
 * @return value or implicit value
 */
    std::string
OptionAgent::getParam(const std::string &name,
        const std::string &implicit) const
{
    return m_environ->getParam(name, implicit);
}
//-----------------------------------------------------------------
/**
 * Returns number value.
 * Implicit value is zero.
 *
 * @param name param name
 * @param implicit default value = 0
 * @return number or implicit
 */
    int
OptionAgent::getAsInt(const std::string &name,
        int implicit) const
{
    return m_environ->getAsInt(name, implicit);
}
//-----------------------------------------------------------------
/**
 * Returns boolean value.
 * Implicit value is false.
 *
 * @param name param name
 * @param implicit default value = false
 * @return stored boolean value or implicit value
 */
    bool
OptionAgent::getAsBool(const std::string &name,
        bool implicit) const
{
    return m_environ->getAsBool(name, implicit);
}
//-----------------------------------------------------------------
/**
 * Set param also on disk.
 * Preserve all current params in options file.
 */
    void
OptionAgent::setPersistent(const std::string &name, const std::string &value)
{
    //NOTE: path must be created before change of environ
    File config = File::external(CONFIG_FILE);

    Environ *swap_env = m_environ;
    m_environ = new Environ();

    if (config.exists()) {
        ScriptAgent::agent()->scriptInclude(config);
    }
    setParam(name, value);
    m_environ->store(config);

    delete m_environ;
    m_environ = swap_env;
    setParam(name, value);
}
//-----------------------------------------------------------------
    void
OptionAgent::setPersistent(const std::string &name, long value)
{
    setPersistent(name, StringTool::toString(value));
}
//-----------------------------------------------------------------
/**
 * Set value when it is empty.
 */
void
OptionAgent::setDefault(const std::string &name, const std::string &value)
{
    m_environ->setParam(name, m_environ->getParam(name, value));
}
//-----------------------------------------------------------------
void
OptionAgent::setDefault(const std::string &name, int value)
{
    m_environ->setParam(name, m_environ->getAsInt(name, value));
}
//-----------------------------------------------------------------
    void
OptionAgent::addWatcher(const std::string &name, BaseMsg *msg)
{
    m_environ->addWatcher(name, msg);
}
//-----------------------------------------------------------------
void
OptionAgent::removeWatchers(const std::string &listenerName)
{
    m_environ->removeWatchers(listenerName);
}

//-----------------------------------------------------------------
void
OptionAgent::readUserConfig()
{
    File userConfig = File::external(CONFIG_FILE);
    if (userConfig.exists()) {
        ScriptAgent::agent()->scriptInclude(userConfig);
    }
}

