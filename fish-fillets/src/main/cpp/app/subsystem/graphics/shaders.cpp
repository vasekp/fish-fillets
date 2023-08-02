#include "subsystem/graphics.h"
#include "subsystem/files.h"

#ifdef FISH_FILLETS_USE_VULKAN
constexpr auto ownPushConstantOffset = sizeof(BaseProgram::Params);
#endif

// NB. we can't get system from instance yet because this function is called when instance().graphics().system() is being constructed.
#ifdef FISH_FILLETS_USE_VULKAN
Shaders::Shaders(Instance& instance, GraphicsSystem& system) :
    m_vert{system.display(), instance.files().system("shader/vulkan/pixel.spv")->read()},
    m_copy{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/copy.spv")->read()}},
    m_maskCopy{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/mask-copy.spv")->read()}},
    m_alpha{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/alpha.spv")->read()}},
    m_reverse{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/reverse.spv")->read()}},
    m_mirror{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/mirror.spv")->read()}},
    m_flat{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/flat.spv")->read()}},
    m_blur{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/blur.spv")->read()}},
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
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/ycbcr.spv")->read()}},
    m_button{system.display(), m_vert,
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/button.spv")->read()}},
    m_arrow{system.display(),
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/arrow.vert.spv")->read()},
        vulkan::Shader{system.display(), instance.files().system("shader/vulkan/arrow.frag.spv")->read()}}
{ }
#else
Shaders::Shaders(Instance& instance, GraphicsSystem& system) :
    m_vert{system.display(), GL_VERTEX_SHADER, instance.files().system("shader/opengl/pixel.vert")->read()},
    m_copy{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/copy.frag")->read()}},
    m_maskCopy{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/mask-copy.frag")->read()}},
    m_alpha{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/alpha.frag")->read()}},
    m_reverse{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/reverse.frag")->read()}},
    m_mirror{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/mirror.frag")->read()}},
    m_flat{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/flat.frag")->read()}},
    m_blur{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/blur.frag")->read()}},
    m_disintegrate{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/disintegrate.frag")->read()}},
    m_wavyImage{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/wavy-image.frag")->read()}},
    m_wavyText{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/wavy-text.frag")->read()}},
    m_titleText{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/title.frag")->read()}},
    m_zx{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/zx.frag")->read()}},
    m_ycbcr{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/ycbcr.frag")->read()}},
    m_button{system.display(), m_vert,
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/button.frag")->read()}},
    m_arrow{system.display(),
        ogl::Shader{system.display(), GL_VERTEX_SHADER, instance.files().system("shader/opengl/arrow.vert")->read()},
        ogl::Shader{system.display(), GL_FRAGMENT_SHADER, instance.files().system("shader/opengl/arrow.frag")->read()}}
{
    for(const auto* program : {&m_copy, &m_maskCopy, &m_alpha, &m_blur, &m_wavyImage, &m_wavyText, &m_titleText, &m_disintegrate, &m_mirror, &m_zx, &m_button}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uSrcTexture"), 0);
    }

    for(const auto* program : {&m_maskCopy, &m_mirror}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uMaskTexture"), 1);
    }

    glUseProgram(m_ycbcr);
    glUniform1i(m_ycbcr.uniform("uCbTexture"), 1);
    glUniform1i(m_ycbcr.uniform("uCrTexture"), 2);
}
#endif

void BaseProgram::run([[maybe_unused]] GraphicsSystem& system, DrawTarget& target, const BaseProgram::Params& params, Shape shape,
    const BaseProgram::Textures& textures) const {
#ifdef FISH_FILLETS_USE_VULKAN
    const auto& display = system.display();
    const auto& commandBuffer = display.commandBuffer();

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_native);

    commandBuffer.pushConstants<Params>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, params);

    std::vector<vk::DescriptorSet> descriptorSets{};
    for(const auto texture : textures)
        descriptorSets.push_back(texture.get().native().descriptorSet());
    if(!descriptorSets.empty())
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_native.pipelineLayout(), 0, descriptorSets, {});

    own_params(system);

    commandBuffer.draw(shape == Shape::rect ? 4 : 3, 1, 0, 0);
#else
    int texIndex = 0;
    for(auto ref : textures) {
        glActiveTexture(GL_TEXTURE0 + texIndex);
        glBindTexture(GL_TEXTURE_2D, ref.get().native());
        texIndex++;
    }
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(m_native);
    glUniform2fv(m_native.uniform("uSrcSize"), 1, params.srcSize.data());
    glUniform2fv(m_native.uniform("uDstSize"), 1, params.dstSize.data());
    glUniform2fv(m_native.uniform("uSrcOffset"), 1, params.src.data());
    glUniform2fv(m_native.uniform("uDstOffset"), 1, params.dest.data());
    glUniform2fv(m_native.uniform("uArea"), 1, params.area.data());
    glUniform4fv(m_native.uniform("uCoords"), 1, params.coords.data());

    own_params(system);

    constexpr static float rect[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    constexpr static float triangle[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

    switch(shape) {
        case Shape::rect: {
            glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &rect[0][0]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            break;
        }
        case Shape::triangle: {
            glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &triangle[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }
    }
#endif
}

#ifdef FISH_FILLETS_USE_VULKAN
template<typename SpecParams>
void Program<SpecParams>::own_params(GraphicsSystem& system) const {
    const auto& commandBuffer = system.display().commandBuffer();
    commandBuffer.pushConstants<SpecParams>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, ownPushConstantOffset, m_params);
}

template class Program<Shaders::MaskCopyParams>;
template class Program<Shaders::AlphaParams>;
template class Program<Shaders::FlatParams>;
template class Program<Shaders::BlurParams>;
template class Program<Shaders::DisintegrateParams>;
template class Program<Shaders::WavyImageParams>;
template class Program<Shaders::WavyTextParams>;
template class Program<Shaders::TitleTextParams>;
template class Program<Shaders::ZXParams>;
template class Program<Shaders::ButtonParams>;
template class Program<Shaders::ArrowParams>;
#else

template<>
void Program<Shaders::AlphaParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform1f(m_native.uniform("uAlpha"), m_params.alpha);
}

template<>
void Program<Shaders::MaskCopyParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uMaskColor"), 1, m_params.maskColor.data());
}

template<>
void Program<Shaders::FlatParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::BlurParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform2fv(m_native.uniform("uDelta"), 1, m_params.dir.data());
}

template<>
void Program<Shaders::DisintegrateParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::WavyImageParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform1f(m_native.uniform("uAmplitude"), m_params.amplitude);
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uPhase"), m_params.phase);
}

template<>
void Program<Shaders::WavyTextParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.data());
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::TitleTextParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::ZXParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.data());
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uOffset"), m_params.offset);
}

template<>
void Program<Shaders::ButtonParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::ArrowParams>::own_params([[maybe_unused]] GraphicsSystem& system) const {
    glUniform2fv(m_native.uniform("uPosition"), 1, m_params.position.data());
    glUniform2fv(m_native.uniform("uDirection"), 1, m_params.direction.data());
    glUniform1f(m_native.uniform("uSize"), m_params.size);
    glUniform1f(m_native.uniform("uSign"), m_params.sign);
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}
#endif
