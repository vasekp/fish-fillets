#ifndef HEADER_WIPICTURE_H
#define HEADER_WIPICTURE_H

class Path;

#include "IWidget.h"

/**
 * Picture widget.
 */
class WiPicture : public IWidget {
    protected:
        SDL_Surface *m_surface;
    protected:
        WiPicture(SDL_Surface *new_surface) { m_surface = new_surface; }
    public:
        WiPicture(const Path &picture);
        virtual ~WiPicture();
        virtual int getW() const { return m_surface->getWidth(); }
        virtual int getH() const { return m_surface->getHeight(); }

        virtual void drawOn(SDL_Surface *screen);
};

#endif
