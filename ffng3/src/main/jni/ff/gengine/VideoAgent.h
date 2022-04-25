#ifndef HEADER_VIDEOAGENT_H
#define HEADER_VIDEOAGENT_H

class File;

#include "common.h"
#include "BaseAgent.h"
#include "MultiDrawer.h"
#include "Name.h"

/**
 * Video agent initializes video mode and
 * every cycle lets registered drawers to drawOn(screen).
 */
class VideoAgent : public BaseAgent, public MultiDrawer {
    AGENT(VideoAgent, Name::VIDEO_NAME);
    private:
        SDL_Surface *m_screen;

private:
        //FFNG void setIcon(const File &file);
        void changeVideoMode(int screen_width, int screen_height);
        int getVideoFlags();

protected:
        virtual void own_init();
        virtual void own_update();
        virtual void own_shutdown();
    public:
        virtual void receiveSimple(const SimpleMsg *msg);
        virtual void receiveString(const StringMsg *msg);

        void initVideoMode();
};

#endif
