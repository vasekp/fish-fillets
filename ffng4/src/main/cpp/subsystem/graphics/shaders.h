#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;
    ogl::Program copyOverlay;
    ogl::Program maskCopy;
    ogl::Program alpha;
    ogl::Program blur;
    ogl::Program wavyImage;
    ogl::Program wavyText;
    ogl::Program titleText;
    ogl::Program disintegrate;

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texMask_shader = 1;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texMask_gl = GL_TEXTURE1;

    Shaders(const std::shared_ptr<ogl::Display>& ref, Instance& instance);
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H