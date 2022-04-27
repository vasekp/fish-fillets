#ifndef HEADER_DUMMYSOUNDAGENT_H
#define HEADER_DUMMYSOUNDAGENT_H

#include "SoundAgent.h"
#include "FFNGMusic.h"

/**
 * NO sound and music.
 */
class DummySoundAgent : public SoundAgent {
    protected:
        virtual void setSoundVolume(int ) {}
        virtual void setMusicVolume(int ) {}
    public:
        virtual int playSound(Mix_Chunk *, int /*volume*/, int /*loops*/)
        { return -1; }

        virtual void playMusic(const File &)
        {
        }
        virtual void stopMusic() {}
};

#endif
