#ifndef HEADER_PICTURE_H
#define HEADER_PICTURE_H

class File;

#include "Drawable.h"
#include "V2.h"

/**
 * Static picture at fixed screen position.
 */
class Picture : public Drawable {
    protected:
        V2 m_loc;
        SDL_Surface *m_surface;
    public:
        Picture(const File &file, const V2 &loc);
        Picture(SDL_Surface *new_surface, const V2 &loc);
        virtual ~Picture();

        int getW() const { return m_surface->getWidth(); }
        int getH() const { return m_surface->getHeight(); }
        virtual void drawOn(SDL_Surface *screen);

        void setLoc(const V2 &loc) { m_loc = loc; }
        void changePicture(const File &file);
        void changePicture(SDL_Surface *new_surface);
};

#endif
