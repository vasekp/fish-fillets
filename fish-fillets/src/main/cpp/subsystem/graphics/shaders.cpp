#include "subsystem/graphics.h"
#include "subsystem/files.h"

Shaders::Shaders(const std::shared_ptr<ogl::Display>& ref, Instance& instance) {
    auto vertCommon = ogl::Shader(ref, GL_VERTEX_SHADER, instance.files().system("shader/pixel.vert")->read());

    copy = ogl::Program(ref, vertCommon,{ref, GL_FRAGMENT_SHADER, instance.files().system("shader/copy.frag")->read()});
    maskCopy = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/mask-copy.frag")->read()});
    alpha = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/alpha.frag")->read()});
    blur = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/blur.frag")->read()});
    wavyImage = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/wavy-image.frag")->read()});
    wavyText = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/wavy-text.frag")->read()});
    titleText = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/title.frag")->read()});
    disintegrate = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/disintegrate.frag")->read()});
    mirror = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/mirror.frag")->read()});
    reverse = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/reverse.frag")->read()});
    flat = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/flat.frag")->read()});
    zx = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/zx.frag")->read()});
    ycbcr = ogl::Program(ref, vertCommon, {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/ycbcr.frag")->read()});

    arrow = ogl::Program(ref,
            {ref, GL_VERTEX_SHADER, instance.files().system("shader/arrow.vert")->read()},
            {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/arrow.frag")->read()});
    button = ogl::Program(ref,
            vertCommon,
            {ref, GL_FRAGMENT_SHADER, instance.files().system("shader/button.frag")->read()});

    for(const auto* program : {&copy, &maskCopy, &alpha, &blur, &wavyImage, &wavyText, &titleText, &disintegrate, &mirror, &zx, &button}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uSrcTexture"), texImage_shader);
    }

    for(const auto* program : {&maskCopy, &mirror}) {
        glUseProgram(*program);
        glUniform1i(maskCopy.uniform("uMaskTexture"), texMask_shader);
    }

    glUseProgram(ycbcr);
    glUniform1i(ycbcr.uniform("uCbTexture"), texCb_shader);
    glUniform1i(ycbcr.uniform("uCrTexture"), texCr_shader);
}

void Shaders::BlurProgram::own_params() const {
    glUniform2f(m_native.uniform("uDelta"), m_dir.fx(), m_dir.fy());
}
