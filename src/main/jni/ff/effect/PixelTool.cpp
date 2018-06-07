/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "PixelTool.h"

#include "LogicException.h"

#include <assert.h>

//-----------------------------------------------------------------
/**
 * Compare colors.
 * NOTE: aplha values are ignored
 */
bool
PixelTool::colorEquals(const Color &color1, const Color &color2)
{
    return color1.r == color2.r
        && color1.g == color2.g
        && color1.b == color2.b;
}
//-----------------------------------------------------------------
Uint32
PixelTool::convertColor(/*FFNG SDL_PixelFormat *format, */const Color &color)
{
    //FFNG always the same format for android... return SDL_MapRGB(format, color.r, color.g, color.b);
	return color.get32();
}
//-----------------------------------------------------------------
/**
 * Get color at x, y.
 * Surface must be locked.
 * @return color
 */
Color
PixelTool::getColor(SDL_Surface *surface, int x, int y)
{
	/*FFNG always the same format for android
    SDL_Color color;
    SDL_GetRGBA(getPixel(surface, x, y), surface->format,
            &color.r, &color.g, &color.b, &color.unused);
    return color;
    */
	Color c(getPixel(surface, x, y));
	return c;
}
//-----------------------------------------------------------------
/**
 * Put color at x, y.
 * Surface must be locked.
 * TODO: support alpha values
 */
void
PixelTool::putColor(SDL_Surface *surface, int x, int y,
        const Color &color)
{
	/*FFNG always the same format for android...
    Uint32 pixel = SDL_MapRGBA(surface->format,
            color.r, color.g, color.b, color.unused);
	*/
    putPixel(surface, x, y, color.get32()/*FFNG pixel*/);
}
//-----------------------------------------------------------------
/**
 * Get pixel at x, y.
 * Surface must be locked.
 * @return pixel
 */
    Uint32
PixelTool::getPixel(SDL_Surface *surface, int x, int y)
{
    assert((0 <= x && x < surface->w) && (0 <= y && y < surface->h));

    /* FFNG no memory access on android
       TODO optimize this by caching whole lines in byte[] (Bitmap)
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch
        + x * bpp;

    return unpackPixel(bpp, p);
    */
    return FFNGSurface::getPixel(surface, x, y);
}
//-----------------------------------------------------------------
/**
 * Put pixel at x, y.
 * Surface must be locked.
 */
    void
PixelTool::putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    if ((0 <= x && x < surface->w) && (0 <= y && y < surface->h)) {
    	/* FFNG no memory access on android
    	   TODO optimize this by using bitmap's byte[]
        int bpp = surface->format->BytesPerPixel;
        Uint8 *p = static_cast<Uint8*>(surface->pixels) + y * surface->pitch
            + x * bpp;

        packPixel(bpp, p, pixel);
        */
    	FFNGSurface::putPixel(surface, x, y, pixel);
    }
}
//-----------------------------------------------------------------
/**
 * Decodes pixel from memory.
 * @param bpp color depth (8, 16, 24, 32)
 * @param p pointer to the memory
 * @return pixel in bpp color depth 
 * @throws LogicException for unknown color depth
 */
    Uint32
PixelTool::unpackPixel(Uint8 bpp, Uint8 *p)
{
    switch(bpp) {
        case 1: // 8bit
            return *p;
        case 2: // 16bit 
            return *reinterpret_cast<Uint16*>(p);
        case 3: // 24bit 
            if (false/*FFNG little endian as default... SDL_BYTEORDER == SDL_BIG_ENDIAN*/) {
                return p[0] << 16 | p[1] << 8 | p[2];
            }
            else {
                return p[0] | p[1] << 8 | p[2] << 16;
            }
        case 4: // 32 bit
            return *reinterpret_cast<Uint32*>(p);
        default:
            throw LogicException(ExInfo("unknown color depth")
                    .addInfo("bpp", bpp));
    }
}
//-----------------------------------------------------------------
/**
 * Encodes pixel to memory.
 * @param bpp color depth (8, 16, 24, 32)
 * @param p pointer to the memory
 * @param pixel prepared pixel in bpp color depth
 * @throws LogicException for unknown color depth
 */
    void
PixelTool::packPixel(Uint8 bpp, Uint8 *p, Uint32 pixel)
{
    assert(p != NULL);

    switch(bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *reinterpret_cast<Uint16*>(p) = pixel;
            break;
        case 3:
            if (false/*FFNG little endian as default... SDL_BYTEORDER == SDL_BIG_ENDIAN*/) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *reinterpret_cast<Uint32*>(p) = pixel;
            break;
        default:
            throw LogicException(ExInfo("unknown color depth")
                    .addInfo("bpp", bpp));
    }
}

