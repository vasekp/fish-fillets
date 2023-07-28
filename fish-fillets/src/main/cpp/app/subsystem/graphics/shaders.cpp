#include "subsystem/graphics.h"
#include "subsystem/files.h"

#ifndef FISH_FILLETS_USE_VULKAN
namespace TexUnits {
    constexpr GLint image_shader = 0;
    constexpr GLint mask_shader = 1;
    constexpr GLint cb_shader = 1;
    constexpr GLint cr_shader = 2;
    constexpr GLint image_gl = GL_TEXTURE0;
    constexpr GLint mask_gl = GL_TEXTURE1;
    constexpr GLint cb_gl = GL_TEXTURE1;
    constexpr GLint cr_gl = GL_TEXTURE2;
};
#endif

#ifdef FISH_FILLETS_USE_VULKAN
Shaders::Shaders(Instance& instance, GraphicsSystem& system) :
    m_vert{system.display(), instance.files().system("shader/vulkan/pixel.spv")->read()},
    m_copy{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/copy.spv")->read()}},
    m_maskCopy{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}}, // TODO
    m_alpha{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/alpha.spv")->read()}},
    m_reverse{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/reverse.spv")->read()}},
    m_mirror{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}}, // TODO
    m_flat{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}},
    m_blur{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}}, // TODO
    m_disintegrate{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/disintegrate.spv")->read()}},
    m_wavyImage{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/wavy-image.spv")->read()}},
    m_wavyText{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/wavy-text.spv")->read()}},
    m_titleText{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/title.spv")->read()}},
    m_zx{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/zx.spv")->read()}},
    m_ycbcr{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}}, // TODO
    m_button{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/button.spv")->read()}},
    m_arrow{system.display(),
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/arrow.vert.spv")->read()},
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/arrow.frag.spv")->read()}}
{ }
#else
Shaders::Shaders(Instance& instance, GraphicsSystem& system) {
    // NB. we can't get system from instance yet because this function is called when instance().graphics().system() is being constructed.
    auto& display = system.display();

    auto vertCommon = ogl::Shader(display, GL_VERTEX_SHADER, instance.files().system("shader/opengl/pixel.vert")->read());

    m_copy = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/copy.frag")->read()});
    m_maskCopy = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/mask-copy.frag")->read()});
    m_alpha = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/alpha.frag")->read()});
    m_blur = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/blur.frag")->read()});
    m_wavyImage = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/wavy-image.frag")->read()});
    m_wavyText = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/wavy-text.frag")->read()});
    m_titleText = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/title.frag")->read()});
    m_disintegrate = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/disintegrate.frag")->read()});
    m_mirror = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/mirror.frag")->read()});
    m_reverse = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/reverse.frag")->read()});
    m_flat = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/flat.frag")->read()});
    m_zx = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/zx.frag")->read()});
    m_ycbcr = ogl::Program(display, vertCommon, {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/ycbcr.frag")->read()});

    m_arrow = ogl::Program(display,
            {display, GL_VERTEX_SHADER, instance.files().system("shader/opengl/arrow.vert")->read()},
            {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/arrow.frag")->read()});
    m_button = ogl::Program(display,
            vertCommon,
            {display, GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/button.frag")->read()});

    for(const auto* program : {&m_copy, &m_maskCopy, &m_alpha, &m_blur, &m_wavyImage, &m_wavyText, &m_titleText, &m_disintegrate, &m_mirror, &m_zx, &m_button}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uSrcTexture"), TexUnits::image_shader);
    }

    for(const auto* program : {&m_maskCopy, &m_mirror}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uMaskTexture"), TexUnits::mask_shader);
    }

    glUseProgram(m_ycbcr);
    glUniform1i(m_ycbcr.uniform("uCbTexture"), TexUnits::cb_shader);
    glUniform1i(m_ycbcr.uniform("uCrTexture"), TexUnits::cr_shader);
}
#endif

template<>
void Program<Shaders::AlphaParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        float uAlpha;
    } constants = { m_params.alpha };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform1f(m_native.uniform("uAlpha"), m_params.alpha);
#endif
}

template<>
void Program<Shaders::MaskCopyParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    glUniform4fv(m_native.uniform("uMaskColor"), 1, m_params.maskColor.gl().data());
    glActiveTexture(TexUnits::mask_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.maskImage.native());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(TexUnits::image_gl);
#endif
}

template<>
void Program<Shaders::MirrorParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    glActiveTexture(TexUnits::mask_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.maskImage.native());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(TexUnits::image_gl);
#endif
}

template<>
void Program<Shaders::FlatParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(16) std::array<float, 4> uColor;
    } constants = { m_params.color.gl(m_params.alpha) };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
#endif
}

template<>
void Program<Shaders::BlurParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    glUniform2f(m_native.uniform("uDelta"), m_params.dir.fx(), m_params.dir.fy());
#endif
}

template<>
void Program<Shaders::DisintegrateParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        float uTime;
    } constants = { m_params.time };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform1f(m_native.uniform("uTime"), m_params.time);
#endif
}

template<>
void Program<Shaders::WavyImageParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        float uAmplitude;
        float uPhase; // TODO order
        float uPeriod;
        float uSpeed;
    } constants = {
        m_params.amplitude,
        m_params.phase,
        m_params.period,
        m_params.speed
    };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform1f(m_native.uniform("uAmplitude"), m_params.amplitude);
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uSpeed"), m_params.speed);
    glUniform1f(m_native.uniform("uPhase"), m_params.phase);
#endif
}

template<>
void Program<Shaders::WavyTextParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(16) std::array<float, 4> uColor1;
        alignas(16) std::array<float, 4> uColor2;
        float uTime;
    } constants = {
        m_params.color1.gl(),
        m_params.color2.gl(),
        m_params.time
    };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.gl().data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.gl().data());
    glUniform1f(m_native.uniform("uTime"), m_params.time);
#endif
}

template<>
void Program<Shaders::TitleTextParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(16) std::array<float, 4> uColor;
    } constants = { m_params.color.gl(m_params.alpha) };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform2f(m_native.uniform("uBlitSize"), m_params.blitSize.fx(), m_params.blitSize.fy()); // TODO
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
#endif
}

template<>
void Program<Shaders::ZXParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(16) std::array<float, 4> uColor1;
        alignas(16) std::array<float, 4> uColor2;
        float uPeriod;
        float uOffset;
    } constants = {
        m_params.color1.gl(),
        m_params.color2.gl(),
        m_params.period,
        m_params.offset
    };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.gl().data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.gl().data());
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uOffset"), m_params.offset);
#endif
}

template<>
void Program<Shaders::YCbCrParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    // TODO
#else
    glActiveTexture(TexUnits::cb_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texCb.native());
    glActiveTexture(TexUnits::cr_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texCr.native());
    glActiveTexture(TexUnits::image_gl);
    glBindTexture(GL_TEXTURE_2D, m_params.texY.native());
#endif
}

template<>
void Program<Shaders::ButtonParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(16) std::array<float, 4> uColor;
        alignas(8) std::array<float, 2> uTexSize;
    } constants = {
        m_params.color.gl(m_params.alpha),
        {m_params.texSize.fx(), m_params.texSize.fy()} // TODO
    };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
    glUniform2f(m_native.uniform("uTexSize"), m_params.texSize.fx(), m_params.texSize.fy());
#endif
}

template<>
void Program<Shaders::ArrowParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& commandBuffer = system.display().commandBuffer();
    struct PushConstants {
        alignas(8) std::array<float, 2> uPosition;
        float uSize;
        alignas(8) std::array<float, 2> uDirection;
        float uSign;
        alignas(16) std::array<float, 4> uColor;
    } constants = {
        {m_params.position.fx(), m_params.position.fy()}, // TODO
        m_params.size,
        {m_params.direction.fx(), m_params.direction.fy()},
        m_params.inwards ? -1.f : 1.f,
        m_params.color.gl(m_params.alpha)
    };
    commandBuffer.pushConstants<PushConstants>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, constants);
#else
    glUniform2f(m_native.uniform("uPosition"), m_params.position.fx(), m_params.position.fy());
    glUniform1f(m_native.uniform("uSize"), m_params.size);
    glUniform2f(m_native.uniform("uDirection"), m_params.direction.fx(), m_params.direction.fy());
    glUniform1f(m_native.uniform("uSign"), m_params.inwards ? -1.f : 1.f);
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.gl(m_params.alpha).data());
#endif
}
