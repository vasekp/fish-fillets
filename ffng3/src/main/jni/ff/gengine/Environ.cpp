/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "Environ.h"

#include "Log.h"
#include "File.h"
#include "BaseMsg.h"
#include "NameException.h"
#include "StringTool.h"

#include <stdio.h>

//-----------------------------------------------------------------
/**
 * Free all remain messages for watchers.
 */
Environ::~Environ()
{
    t_watchers::iterator end = m_watchers.end();
    for (t_watchers::iterator i = m_watchers.begin(); i != end; ++i) {
        delete i->second;
    }
}
//-----------------------------------------------------------------
/**
 * Save params.
 * @param file where to store params, this file will be overwritten
 */
    void
Environ::store(const File &file)
{
    /* FFNG
    FILE *config = fopen(file.getPath().c_str(), "w");
    if (config) {
        fputs("-- this file is automatically generated\n", config);

        t_values::iterator end = m_values.end();
        for (t_values::iterator i = m_values.begin(); i != end; ++i) {
            fprintf(config, "setParam(\"%s\", \"%s\")\n",
                    i->first.c_str(), i->second.c_str());
        }

        fclose(config);
    }
    else {
    */
    std::string data = "-- this file is automatically generated\n";
    t_values::iterator end = m_values.end();
    for (t_values::iterator i = m_values.begin(); i != end; ++i) {
        data += "setParam(\"" + i->first + "\", \"" + i->second + "\")\n";
    }

    if (!file.write(data)) {
        LOG_WARNING(ExInfo("cannot save config")
                .addInfo("file", file.getPath()));
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
Environ::setParam(const std::string &name, const std::string &value)
{
    if (m_values[name] != value) {
        m_values[name] = value;
        LOG_DEBUG(ExInfo("setParam")
                .addInfo("param", name)
                .addInfo("value", value));

        t_watchers::iterator it = m_watchers.find(name);
        if (m_watchers.end() != it) {
            t_watchers::size_type count = m_watchers.count(name);
            for (t_watchers::size_type i = 0; i < count; ++i) {
                t_watchers::iterator cur_it = it++;
                try {
                    cur_it->second->sendClone();
                }
                catch (NameException &e) {
                    LOG_WARNING(e.info());
                    delete cur_it->second;
                    m_watchers.erase(cur_it);
                }
            }
        }
    }
}
//-----------------------------------------------------------------
/**
 * Store this integer value like string param.
 * @param name param name
 * @param value param value
 */
    void
Environ::setParam(const std::string &name, long value)
{
    setParam(name, StringTool::toString(value));
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
Environ::getParam(const std::string &name,
                const std::string &implicit) const
{
    std::string result = implicit;

    t_values::const_iterator it = m_values.find(name);
    if (m_values.end() != it) {
        result = it->second;
    }
    return result;
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
Environ::getAsInt(const std::string &name,
                int implicit) const
{
    std::string value = getParam(name);
    bool ok;
    int result = StringTool::readInt(value.c_str(), &ok);
    if (!ok) {
        if (value != "") {
            LOG_WARNING(ExInfo("cannot recognize numeric value")
                    .addInfo("property", name)
                    .addInfo("value", value)
                    .addInfo("default", implicit));
        }
        result = implicit;
    }
    return result;
}
//-----------------------------------------------------------------
/**
 * Returns boolean value.
 * Recognizes 1/0, true/false, on/off, yes/no.
 *
 * @param name param name
 * @param implicit default value = false
 * @return stored value or implicit value when value is not recognized
 */
    bool
Environ::getAsBool(const std::string &name,
                bool implicit) const
{
    bool result = false;
    std::string value = getParam(name);
    if (value == "1" || value == "true" || value == "on" || value == "yes") {
        result = true;
    }
    else if (value == "0" || value == "false" || value == "off" ||
            value == "no")
    {
        result = false;
    }
    else {
        if (value != "") {
            //TODO: don't print this every time
            LOG_WARNING(ExInfo("cannot recognize boolean value")
                    .addInfo("property", name)
                    .addInfo("value", value)
                    .addInfo("default", implicit)
                    .addInfo("hint", "use 1/0, true/false, on/off, yes/no"));
        }
        result = implicit;
    }
    return result;
}
//-----------------------------------------------------------------
/**
 * Multiple watcher can watch param change.
 * @param name param name
 * @param msg message to raise
 */
    void
Environ::addWatcher(const std::string &name, BaseMsg *msg)
{
    m_watchers.insert(std::pair<std::string,BaseMsg*>(name, msg));
    LOG_DEBUG(ExInfo("add watcher")
            .addInfo("param", name)
            .addInfo("msg", msg->toString()));
}
//-----------------------------------------------------------------
/**
 * Removes all registered watchers for given listener.
 */
void
Environ::removeWatchers(const std::string &listenerName)
{
    t_watchers::iterator end = m_watchers.end();
    for (t_watchers::iterator i = m_watchers.begin(); i != end; /*empty*/) {
        t_watchers::iterator cur = i++;
        if (cur->second->getListenerName() == listenerName) {
            delete cur->second;
            m_watchers.erase(cur);
        }
    }
}
//-----------------------------------------------------------------
std::string
Environ::toString() const
{
    ExInfo info("environ");
    t_values::const_iterator end = m_values.end();
    for (t_values::const_iterator i = m_values.begin(); i != end; ++i) {
        info.addInfo(i->first, i->second);
    }
    return info.info();
}
//-----------------------------------------------------------------
std::string
Environ::getHelpInfo() const
{
    std::string help;
    t_values::const_iterator end = m_values.end();
    for (t_values::const_iterator i = m_values.begin(); i != end; ++i) {
        help += i->first + "='" + i->second + "'\n";
    }
    return help;
}

