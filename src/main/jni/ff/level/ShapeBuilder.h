#ifndef HEADER_SHAPE_BUILDER_H
#define HEADER_SHAPE_BUILDER_H

class Shape;

#include "Cube.h"

//FFNG #include "SDL.h"
#include "FFNGSurface.h"
#include "Color.h"

/**
 * Creator of images from shapes.
 * Useable for testing of new levels.
 */
class ShapeBuilder {
    private:
        /**
        * ShapeBuilder is not meant to be instantiated.
         */
        ShapeBuilder() {}
        static void prepareColor(Color *color, const Shape *shape,
                Cube::eWeight weight);
        static Uint32 calcShapeHash(const Shape *shape);
    public:
        static SDL_Surface *createImage(const Shape *shape, Cube::eWeight weight);
};

#endif
