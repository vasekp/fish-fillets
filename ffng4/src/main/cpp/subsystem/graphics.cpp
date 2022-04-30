#include "graphics.h"
#include "decoders.h"

Graphics::Graphics(Instance* instance) {
    display = std::make_unique<ogl::Display>(instance->app->window);

    canvas = std::make_unique<Canvas>(1000, 1000);

    shaders = std::make_unique<Shaders>(*instance);

    bg = std::make_unique<ogl::Texture>(instance->decoders->loadImage("orig/map.png"));
}