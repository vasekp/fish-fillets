#ifndef HEADER_SURFACELOCK_H
#define HEADER_SURFACELOCK_H

#include "NoCopy.h"

//FFNG #include "SDL.h"
#include "FFNGSurface.h"

/**
 * Lock and unlock surface.
 */
class SurfaceLock : public NoCopy {
    private:
        SDL_Surface *m_surface;
    public:
        SurfaceLock(SDL_Surface *surface);
        virtual ~SurfaceLock();
};

#endif
