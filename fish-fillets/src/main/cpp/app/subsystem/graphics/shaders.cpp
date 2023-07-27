#include "subsystem/graphics.h"
#include "subsystem/files.h"

Shaders::Shaders(Instance& instance, GraphicsSystem& system) {
    // NB. we can't get system from instance yet because this function is called when instance().graphics().system() is being constructed.
    auto& display = system.display();
    auto vertCommon = ogl::Shader(display, GL_VERTEX_SHADER, instance.files().system("shader/pixel.vert")->read());

    m_copy = ogl::Program(display, vertCommon,{display, GL_FRAGMENT_SHADER, instance.files().system("shader/copy.frag")->read()});
    m_maskCopy = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/mask-copy.frag")->read()});
    m_alpha = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/alpha.frag")->read()});
    m_blur = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/blur.frag")->read()});
    m_wavyImage = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/wavy-image.frag")->read()});
    m_wavyText = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/wavy-text.frag")->read()});
    m_titleText = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/title.frag")->read()});
    m_disintegrate = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/disintegrate.frag")->read()});
    m_mirror = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/mirror.frag")->read()});
    m_reverse = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/reverse.frag")->read()});
    m_flat = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/flat.frag")->read()});
    m_zx = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/zx.frag")->read()});
    m_ycbcr = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/ycbcr.frag")->read()});

    m_arrow = ogl::Program(display,
            {display, GL_VERTEX_SHADER, instance.files().system("shader/arrow.vert")->read()},
            {display, GL_FRAGMENT_SHADER, instance.files().system("shader/arrow.frag")->read()});
    m_button = ogl::Program(display,
            vertCommon,
            {display, GL_FRAGMENT_SHADER, instance.files().system("shader/button.frag")->read()});

    for(const auto* program : {&m_copy, &m_maskCopy, &m_alpha, &m_blur, &m_wavyImage, &m_wavyText, &m_titleText, &m_disintegrate, &m_mirror, &m_zx, &m_button}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uSrcTexture"), texImage_shader);
    }

    for(const auto* program : {&m_maskCopy, &m_mirror}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uMaskTexture"), texMask_shader);
    }

    glUseProgram(m_ycbcr);
    glUniform1i(m_ycbcr.uniform("uCbTexture"), texCb_shader);
    glUniform1i(m_ycbcr.uniform("uCrTexture"), texCr_shader);
}

template<>
void Program<Shaders::AlphaParams>::own_params() const {
    glUniform1f(m_native.uniform("uAlpha"), m_params.alpha);
}

template<>
void Program<Shaders::MaskCopyParams>::own_params() const {
    glUniform4fv(m_native.uniform("uMaskColor"), 1, m_params.maskColor.gl().data());
    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.maskImage.native());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(Shaders::texImage_gl);
}

template<>
void Program<Shaders::MirrorParams>::own_params() const {
    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.maskImage.native());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(Shaders::texImage_gl);
}

template<>
void Program<Shaders::FlatParams>::own_params() const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
}

template<>
void Program<Shaders::BlurParams>::own_params() const {
    glUniform2f(m_native.uniform("uDelta"), m_params.dir.fx(), m_params.dir.fy());
}

template<>
void Program<Shaders::DisintegrateParams>::own_params() const {
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::WavyImageParams>::own_params() const {
    glUniform1f(m_native.uniform("uAmplitude"), m_params.amplitude);
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uSpeed"), m_params.speed);
    glUniform1f(m_native.uniform("uPhase"), m_params.phase);
}

template<>
void Program<Shaders::WavyTextParams>::own_params() const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.gl().data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.gl().data());
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::TitleTextParams>::own_params() const {
    glUniform2f(m_native.uniform("uBlitSize"), m_params.blitSize.fx(), m_params.blitSize.fy());
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
}

template<>
void Program<Shaders::ZXParams>::own_params() const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.gl().data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.gl().data());
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uOffset"), m_params.offset);
}

template<>
void Program<Shaders::YCbCrParams>::own_params() const {
    glActiveTexture(Shaders::texCb_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texCb.native());
    glActiveTexture(Shaders::texCr_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texCr.native());
    glActiveTexture(Shaders::texImage_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texY.native());
}

template<>
void Program<Shaders::ButtonParams>::own_params() const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
    glUniform2f(m_native.uniform("uTexSize"), m_params.texSize.fx(), m_params.texSize.fy());
}

template<>
void Program<Shaders::ArrowParams>::own_params() const {
    glUniform2f(m_native.uniform("uPosition"), m_params.position.fx(), m_params.position.fy());
    glUniform1f(m_native.uniform("uSize"), m_params.size);
    glUniform2f(m_native.uniform("uDirection"), m_params.direction.fx(), m_params.direction.fy());
    glUniform1f(m_native.uniform("uSign"), m_params.inwards ? -1.f : 1.f);
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
}