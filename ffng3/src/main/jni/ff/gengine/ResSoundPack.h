#ifndef HEADER_RESSOUNDPACK_H
#define HEADER_RESSOUNDPACK_H

class File;

#include "ResourcePack.h"

//FFNG #include "SDL_mixer.h"
#include "FFNGMusic.h"

/**
 * Sound resources.
 */
class ResSoundPack : public ResourcePack<Mix_Chunk*> {
    public:
        virtual const char *getName() const { return "sound_pack"; }

        static Mix_Chunk *loadSound(const File &file);
        void addSound(const std::string &name, const File &file);
        virtual void unloadRes(Mix_Chunk *res);
};

#endif

