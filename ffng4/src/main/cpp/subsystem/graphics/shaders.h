#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

struct Shaders {
    ogl::Shader vertCommon;
    ogl::Program copy;
    ogl::Program maskCopy;
    ogl::Program alpha;
    ogl::Program wavyImage;

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texMask_shader = 1;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texMask_gl = GL_TEXTURE1;

    Shaders(const std::shared_ptr<ogl::Display>& ref, Instance& instance) {
        vertCommon = ogl::Shader(ref, GL_VERTEX_SHADER, instance.files().system("shader/pixel.vert").read());

        copy = ogl::Program(ref, vertCommon,{ref, GL_FRAGMENT_SHADER, instance.files().system("shader/copy.frag").read()});
        maskCopy = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/mask-copy.frag").read()});
        alpha = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/alpha.frag").read()});
        wavyImage = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/wavy-image.frag").read()});

        for(const auto* program : {&copy, &maskCopy, &alpha, &wavyImage}) {
            glUseProgram(*program);
            glUniform1i(program->uniform("uTexture"), Shaders::texImage_shader);
            glUniform2f(program->uniform("uSrcOffset"), 0.f, 0.f);
        }

        glUseProgram(maskCopy);
        glUniform1i(maskCopy.uniform("uMaskTexture"), Shaders::texMask_shader);
    }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H