#include "graphics.h"
#include "decoders.h"

Graphics::Graphics(Instance* instance) {
    display = std::make_unique<ogl::Display>(instance->app->window);

    canvas = std::make_unique<ogl::Framebuffer>(1000, 1000);
    glActiveTexture(Shaders::texCanvas_gl);
    glBindTexture(GL_TEXTURE_2D, canvas->texture());
    glActiveTexture(Shaders::texImage_gl);

    shaders = std::make_unique<Shaders>(*instance);

    bg = std::make_unique<ogl::Texture>(instance->decoders->loadImage("orig/map.png"));
}