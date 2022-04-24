/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "BaseMsg.h"

#include "MessagerAgent.h"

//-----------------------------------------------------------------
/**
 * Create msg.
 * @param listenerName name of destination
 * @param name msg name
 */
BaseMsg::BaseMsg(const std::string &listenerName, const std::string &name)
: m_listenerName(listenerName), m_name(name)
{
    /* empty */
}
//-----------------------------------------------------------------
/**
 * Send copy of message.
 */
void
BaseMsg::send() const
{
    //NOTE: message type must be preserved
    MessagerAgent::agent()->forwardMsg(*this);
}
//-----------------------------------------------------------------
std::string
BaseMsg::toString() const
{
    return m_listenerName + " -> " + m_name;
}
