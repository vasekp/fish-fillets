#ifndef HEADER_PIXELTOOL_H
#define HEADER_PIXELTOOL_H

//#include "SDL.h"
#include "Color.h"
#include "FFNGSurface.h"

/**
 * Pixel rutines.
 */
class PixelTool {
    public:
        static Uint32 convertColor(/*FFNGSDL_PixelFormat *format,*/
                const Color &color);
        static Uint32 getPixel(SDL_Surface *surface, int x, int y);
};

#endif
