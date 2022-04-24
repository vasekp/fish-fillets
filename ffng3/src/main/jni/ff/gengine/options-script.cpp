/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "options-script.h"

#include "OptionAgent.h"
#include "MessagerAgent.h"
#include "StringMsg.h"
#include "IntMsg.h"
#include "SimpleMsg.h"
#include "def-script.h"


//-----------------------------------------------------------------
/**
 * void sendMsg(listener, msg, [value])
 */
    int
script_options_sendMsg(lua_State *L) throw()
{
    BEGIN_NOEXCEPTION;
    const char *listener = luaL_checkstring(L, 1);
    const char *msg = luaL_checkstring(L, 2);
    auto *agent = MessagerAgent::agent();
    if (lua_isstring(L, 3)) {
        const char *string_value = luaL_checkstring(L, 3);
        agent->forwardMsg(StringMsg(listener, msg, string_value));
    }
    else if (lua_isnumber(L, 3)) {
        int int_value = luaL_checkint(L, 3);
        agent->forwardMsg(IntMsg(listener, msg, int_value));
    }
    else {
        agent->forwardMsg(SimpleMsg(listener, msg));
    }

    END_NOEXCEPTION;
    return 0;
}
//-----------------------------------------------------------------
/**
 * void setParam(name, value)
 */
    int
script_options_setParam(lua_State *L) throw()
{
    BEGIN_NOEXCEPTION;
    const char *name = luaL_checkstring(L, 1);
    const char *value = lua_tostring(L, 2);
    if (!value) {
        value = luaL_checkstring(L, 2);
    }
    OptionAgent::agent()->setParam(name, value);
    END_NOEXCEPTION;
    return 0;
}
//-----------------------------------------------------------------
/**
 * string getParam(name)
 * Returns string or nil.
 */
    int
script_options_getParam(lua_State *L) throw()
{
    BEGIN_NOEXCEPTION;
    const char *name = luaL_checkstring(L, 1);
    std::string value = OptionAgent::agent()->getParam(name);
    if (value.empty()) {
        lua_pushnil(L);
    }
    else {
        lua_pushlstring(L, value.c_str(), value.size());
    }
    END_NOEXCEPTION;
    //NOTE: return value
    return 1;
}

