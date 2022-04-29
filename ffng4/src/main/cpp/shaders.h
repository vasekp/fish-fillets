#ifndef FISH_FILLETS_SHADERS_H
#define FISH_FILLETS_SHADERS_H

#include "instance.h"
#include "ogl.h"

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;

    Shaders(const Instance& instance) :
        vertCommon(GL_VERTEX_SHADER, instance.systemFile("shader/united.vert").read()),
        copy(vertCommon, {GL_FRAGMENT_SHADER, instance.systemFile("shader/copy.frag").read()})
    { }
};

#endif //FISH_FILLETS_SHADERS_H
