#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;
    ogl::Program maskCopy;
    ogl::Program alpha;
    ogl::Program wavyImage;
    ogl::Program fill;

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texCanvas_shader = 1;
    constexpr static GLint texMask_shader = 2;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texCanvas_gl = GL_TEXTURE1;
    constexpr static GLint texMask_gl = GL_TEXTURE2;

    Shaders(const Instance* instance) {
        vertCommon = ogl::Shader(GL_VERTEX_SHADER, instance->files->system("shader/pixel.vert").read());

        copy = ogl::Program(vertCommon, {GL_FRAGMENT_SHADER, instance->files->system("shader/copy.frag").read()});
        maskCopy = ogl::Program(vertCommon, {GL_FRAGMENT_SHADER, instance->files->system("shader/mask-copy.frag").read()});
        alpha = ogl::Program(vertCommon, {GL_FRAGMENT_SHADER, instance->files->system("shader/alpha.frag").read()});
        wavyImage = ogl::Program(vertCommon, {GL_FRAGMENT_SHADER, instance->files->system("shader/wavy-image.frag").read()});

        for(const auto* program : {&copy, &maskCopy, &alpha, &wavyImage}) {
            glUseProgram(*program);
            glUniform1i(program->uniform("uTexture"), Shaders::texImage_shader);
            glUniform2f(program->uniform("uSrcOffset"), 0.f, 0.f);
        }

        glUseProgram(maskCopy);
        glUniform1i(maskCopy.uniform("uMaskTexture"), Shaders::texMask_shader);

        fill = ogl::Program({GL_VERTEX_SHADER, instance->files->system("shader/fill.vert").read()},
             {GL_FRAGMENT_SHADER, instance->files->system("shader/fill.frag").read()});

        glUseProgram(fill);
        glUniform1i(fill.uniform("uScreenTexture"), Shaders::texCanvas_shader);
        glUniform2f(fill.uniform("uTextureSize"), (float) Canvas::texWidth, (float) Canvas::texHeight);
    }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H