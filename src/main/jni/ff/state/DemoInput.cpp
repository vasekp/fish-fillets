/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "DemoInput.h"

#include "Keymap.h"
#include "KeyDesc.h"
#include "GameState.h"

//-----------------------------------------------------------------
/**
 * Create demo input handler.
 * Escape, space or mouse click quit state.
 * @param demo pointer to the leader
 */
DemoInput::DemoInput(GameState *demo)
    : GameInput(demo)
{
	/* FFNG TODO register key (a touch or button) to quit demo
    KeyDesc key_quit(KEY_QUIT, "quit");
    m_keymap->registerKey(KeyStroke(SDLK_SPACE, KMOD_NONE), key_quit);
    */
}
//-----------------------------------------------------------------
void
DemoInput::mouseEvent(const MouseStroke &/*buttons*/)
{
    quitState();
}

