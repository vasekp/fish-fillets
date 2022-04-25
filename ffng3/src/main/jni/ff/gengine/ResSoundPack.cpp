/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "ResSoundPack.h"

#include "File.h"
#include "OptionAgent.h"

//-----------------------------------------------------------------
    void
ResSoundPack::unloadRes(Mix_Chunk *res)
{
    //FFNG Mix_FreeChunk(res);
    FFNGMusic::free(res);
}
//-----------------------------------------------------------------
/**
 * Load unshared sound from file.
 * @return sound or NULL
 */
    Mix_Chunk *
ResSoundPack::loadSound(const File &file)
{
    Mix_Chunk *chunk = NULL;
    //TODO: ask SoundAgent to load this sound
    if (OptionAgent::agent()->getAsBool("sound", true)) {
        chunk = FFNGMusic::loadWAV /*FFNG Mix_LoadWAV*/(file.getPath().c_str());
        if (NULL == chunk) {
            Log::warn("cannot load sound %s", file.getPath().c_str());
        }
    }
    return chunk;
}
//-----------------------------------------------------------------
/**
 * Store sound under this name.
 * Nothing is stored when sound cannot be loaded.
 */
    void
ResSoundPack::addSound(const std::string &name, const File &file)
{
    Mix_Chunk *chunk = loadSound(file);
    if (chunk) {
        addRes(name, chunk);
    }
}

