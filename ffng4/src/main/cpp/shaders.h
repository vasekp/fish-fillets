#ifndef FISH_FILLETS_SHADERS_H
#define FISH_FILLETS_SHADERS_H

#include "instance.h"
#include "ogl.h"

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;
    ogl::Program fill;

    constexpr static GLint aPosition = ogl::Program::aPosition;

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texCanvas_shader = 1;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texCanvas_gl = GL_TEXTURE1;

    Shaders(const Instance& instance) :
        vertCommon(GL_VERTEX_SHADER, instance.systemFile("shader/pixel.vert").read()),
        copy(vertCommon, {GL_FRAGMENT_SHADER, instance.systemFile("shader/copy.frag").read()}),
        fill({GL_VERTEX_SHADER, instance.systemFile("shader/fill.vert").read()},
             {GL_FRAGMENT_SHADER, instance.systemFile("shader/fill.frag").read()})
    { }
};

#endif //FISH_FILLETS_SHADERS_H
