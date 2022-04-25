/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "SurfaceTool.h"
#include "PixelTool.h"    // FFNG

#include <stdexcept>

//-----------------------------------------------------------------
/**
 * Return new empty surface with the same format.
 * @param surface surface to ask for pixel format
 * @param width width or 0 for the same width
 * @param height height or 0 for the same height
 * @throws SDLException when function fails
 */
    SDL_Surface *
SurfaceTool::createEmpty(SDL_Surface *surface, int width, int height)
{
    if (!width) {
        width = surface->getWidth();
    }
    if (!height) {
        height = surface->getHeight();
    }
    SDL_Surface *result = FFNGSurface::createSurface(width, height);
    if (!result) {
        throw std::runtime_error("createSurface returned null");
    }
    return result;
}
//-----------------------------------------------------------------
/**
 * Return new surface with transparent background.
 */
    SDL_Surface *
SurfaceTool::createTransparent(int w, int h, const Color &transparent)
{
    SDL_Surface *surface = FFNGSurface::createSurface(w, h, PixelTool::convertColor(transparent));
    if (!surface) {
        throw std::runtime_error("createSurface returned null");
    }
    return surface;
}
//-----------------------------------------------------------------
/**
 * Return new cloned surface.
 * @return new surface, free it after use
 * @throws SDLException when function fails
 */
#if 0
    SDL_Surface *
SurfaceTool::createClone(SDL_Surface *surface)
{
    /* FFNG
    SDL_Surface *clone = SDL_ConvertSurface(surface,
            surface->format, surface->flags);
    */
    	SDL_Surface *clone = FFNGSurface::clone(surface);
    if (NULL == clone) {
        throw SDLException(ExInfo("ConvertSurface"));
    }
    return clone;
}
#endif
//-----------------------------------------------------------------
/**
 * Fill surface with given color. Alpha value in color is supported.
 * The final blit rectangle is saved in dstrect.
 *
 * @param surface surface to fill
 * @param dstrect dstrect or NULL (the whole surface will be filled with color).
 * @param color {red, green, blue, alpha}
 */
    void
SurfaceTool::alphaFill(SDL_Surface *surface, SDL_Rect *dstrect,
        const Color &color)
{
    int w = surface->getWidth();
    int h = surface->getHeight();
    if (dstrect) {
        w = dstrect->w;
        h = dstrect->h;
    }
    /* FFNG
    SDL_Surface *canvas = createEmpty(surface, w, h);
    Uint32 pixel = SDL_MapRGB(canvas->format, color.r, color.g, color.b);
    SDL_FillRect(canvas, NULL, pixel);
    SDL_SetAlpha(canvas, SDL_SRCALPHA|SDL_RLEACCEL, color.unused);

    SDL_BlitSurface(canvas, NULL, surface, dstrect);
    SDL_FreeSurface(canvas);
    */
    FFNGSurface::fillRect(surface, dstrect, PixelTool::convertColor(color));
}

