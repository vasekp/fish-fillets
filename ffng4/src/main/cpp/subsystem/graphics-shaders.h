#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;
    ogl::Program fill;

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texCanvas_shader = 1;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texCanvas_gl = GL_TEXTURE1;

    Shaders(const Instance& instance) {
        vertCommon = ogl::Shader(GL_VERTEX_SHADER, instance.files->system("shader/pixel.vert").read());
        copy = ogl::Program(vertCommon, {GL_FRAGMENT_SHADER, instance.files->system("shader/copy.frag").read()});
        fill = ogl::Program({GL_VERTEX_SHADER, instance.files->system("shader/fill.vert").read()},
             {GL_FRAGMENT_SHADER, instance.files->system("shader/fill.frag").read()});
    }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H