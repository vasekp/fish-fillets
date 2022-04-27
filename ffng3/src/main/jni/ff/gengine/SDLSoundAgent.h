#ifndef HEADER_SDLSOUNDAGENT_H
#define HEADER_SDLSOUNDAGENT_H

#include "SoundAgent.h"
#include "FFNGMusic.h"

/**
 * Sound and music.
 */
class SDLSoundAgent : public SoundAgent {
    private:
    Mix_Music *m_music;
    std::string m_playingPath;
    int m_soundVolume;

protected:
        virtual void own_init();
        virtual void own_update();
        virtual void own_shutdown();
        virtual void reinit();

        virtual void setSoundVolume(int volume);
        virtual void setMusicVolume(int volume);
    public:
        virtual int playSound(Mix_Chunk *sound, int volume, int loops=0);

        virtual void playMusic(const File &file);
        virtual void stopMusic();
};

#endif
