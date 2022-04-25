/*
 * Copyright (C) 2004 Ivo Danihelka (ivo@danihelka.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
#include "LayeredPicture.h"

#include "File.h"
#include "ResImagePack.h"
#include "SurfaceLock.h"
#include "PixelTool.h"

//-----------------------------------------------------------------
/**
 * Create picture with two layers and color mask to select
 * active areas.
 *
 * @throws ResourceException when lowerLayer and colorMask have
 * different proportions
 */
LayeredPicture::LayeredPicture(const File &bg_file, const V2 &loc,
        const File &lowerLayer, const File &colorMask)
: Picture(bg_file, loc)
{
    m_lowerLayer = ResImagePack::loadImage(lowerLayer);
    m_colorMask = ResImagePack::loadImage(colorMask);
    if (m_lowerLayer->getWidth() != m_colorMask->getWidth()
            || m_lowerLayer->getHeight() != m_colorMask->getHeight()) {
        FFNGSurface::freeSurface(m_lowerLayer);
        FFNGSurface::freeSurface(m_colorMask);
        FFNGSurface::freeSurface(m_surface);

        throw std::logic_error("lowerLayer and colorMask have different proportions: " + lowerLayer.getPath() + ", " + colorMask.getPath());
    }

    setNoActive();
}
//-----------------------------------------------------------------
LayeredPicture::~LayeredPicture()
{
	FFNGSurface::freeSurface(m_lowerLayer);
	FFNGSurface::freeSurface(m_colorMask);
}
//-----------------------------------------------------------------
/**
 * Return pixel at worldLoc.
 * Translates world coordinates to local coordinates.
 */
    Uint32
LayeredPicture::getMaskAtWorld(const V2 &worldLoc)
{
    V2 localLoc = worldLoc.minus(m_loc);
    return getMaskAt(localLoc);
}
//-----------------------------------------------------------------
/**
 * Return pixel at position from left top image corner.
 */
    Uint32
LayeredPicture::getMaskAt(const V2 &loc)
{
    Uint32 result = MASK_NO;

    if ((0 <= loc.getX() && loc.getX() < m_colorMask->getWidth())
            && (0 <= loc.getY() && loc.getY() < m_colorMask->getHeight()))
    {
        SurfaceLock lock1(m_colorMask);
        result = PixelTool::getPixel(m_colorMask,
                loc.getX(), loc.getY());
    }
    return result;
}
//-----------------------------------------------------------------
    void
LayeredPicture::drawOn(SDL_Surface *screen)
{
    Picture::drawOn(screen);
    if (m_activeColor == MASK_NO) {
        return;
    }

    SurfaceLock lock1(screen);
    SurfaceLock lock2(m_lowerLayer);
    SurfaceLock lock3(m_colorMask);

    //TODO: support alpha channels
    screen->blitMasked(m_loc.getX(), m_loc.getY(), m_colorMask, m_activeColor, m_lowerLayer);
}

