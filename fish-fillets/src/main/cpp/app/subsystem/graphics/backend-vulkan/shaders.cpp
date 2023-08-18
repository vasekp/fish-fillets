#include "subsystem/graphics.h"
#include "subsystem/graphics/graphicsbackend.h"
#include "subsystem/files.h"

static constexpr auto ownPushConstantOffset = sizeof(BaseProgram::Params);

struct Shaders::Impl {
    BACKEND::Program copy;
    BACKEND::Program overlay;
    BACKEND::Program maskCopy;
    BACKEND::Program alpha;
    BACKEND::Program reverse;
    BACKEND::Program mirror;
    BACKEND::Program flat;
    BACKEND::Program blur;
    BACKEND::Program disintegrate;
    BACKEND::Program wavyImage;
    BACKEND::Program wavyText;
    BACKEND::Program titleText;
    BACKEND::Program zx;
    BACKEND::Program ycbcr;
    BACKEND::Program button;
    BACKEND::Program arrow;
};

Shaders::~Shaders() = default;

// NB. we can't get backend from instance.graphics().backend() yet because this function is called when Instance is being constructed.
Shaders::Shaders(Instance& instance, GraphicsBackend& backend) {
    auto& display = backend.display();
    auto& files = instance.files();
    auto spirv = [&](std::string filename) {
        return vulkan::Shader{display, files.system("shader/vulkan/"s + filename)->read()};
    };
    vulkan::Shader pixelVert = spirv("pixel.spv");
    pImpl = std::make_unique<Impl>(
        vulkan::Program{display, pixelVert, spirv("copy.spv")},
        vulkan::Program{display, pixelVert, spirv("overlay.spv")},
        vulkan::Program{display, pixelVert, spirv("mask-copy.spv")},
        vulkan::Program{display, pixelVert, spirv("alpha.spv")},
        vulkan::Program{display, pixelVert, spirv("reverse.spv")},
        vulkan::Program{display, pixelVert, spirv("mirror.spv")},
        vulkan::Program{display, pixelVert, spirv("flat.spv")},
        vulkan::Program{display, pixelVert, spirv("blur.spv")},
        vulkan::Program{display, pixelVert, spirv("disintegrate.spv")},
        vulkan::Program{display, pixelVert, spirv("wavy-image.spv")},
        vulkan::Program{display, pixelVert, spirv("wavy-text.spv")},
        vulkan::Program{display, pixelVert, spirv("title.spv")},
        vulkan::Program{display, pixelVert, spirv("zx.spv")},
        vulkan::Program{display, pixelVert, spirv("ycbcr.spv")},
        vulkan::Program{display, pixelVert, spirv("button.spv")},
        vulkan::Program{display, spirv("arrow.vert.spv"), spirv("arrow.frag.spv")}
    );
}

void BaseProgram::run(GraphicsBackend& backend, DrawTarget& target, const BaseProgram::Params& params, Shape shape,
        const BaseProgram::Textures& textures) const {
    const auto& display = backend.display();
    const auto& commandBuffer = display.commandBuffer();

    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_native);

    commandBuffer.pushConstants<Params>(m_native.pipelineLayout(), vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0, params);

    std::vector<vk::DescriptorSet> descriptorSets{};
    for(const auto texture : textures)
        descriptorSets.push_back(texture.get().native().descriptorSet());
    if(!descriptorSets.empty())
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_native.pipelineLayout(), 0, descriptorSets, {});

    own_params(backend);

    commandBuffer.draw(shape == Shape::rect ? 4 : 3, 1, 0, 0);
}

template<typename SpecParams>
void Program<SpecParams>::own_params(GraphicsBackend& backend) const {
    const auto& commandBuffer = backend.display().commandBuffer();
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

BaseProgram Shaders::copy() { return {pImpl->copy}; }

BaseProgram Shaders::overlay() { return {pImpl->overlay}; }

Program<Shaders::MaskCopyParams> Shaders::maskCopy(MaskCopyParams params) { return {pImpl->maskCopy, params}; }

BaseProgram Shaders::reverse() { return {pImpl->reverse}; }

BaseProgram Shaders::mirror() { return {pImpl->mirror}; }

Program<Shaders::AlphaParams> Shaders::alpha(AlphaParams params) { return {pImpl->alpha, params}; }

Program<Shaders::FlatParams> Shaders::flat(FlatParams params) { return {pImpl->flat, params}; }

Program<Shaders::BlurParams> Shaders::blur(BlurParams params) { return {pImpl->blur, params}; }

Program<Shaders::DisintegrateParams> Shaders::disintegrate(DisintegrateParams params) { return {pImpl->disintegrate, params}; }

Program<Shaders::WavyImageParams> Shaders::wavyImage(WavyImageParams params) { return {pImpl->wavyImage, params}; }

Program<Shaders::WavyTextParams> Shaders::wavyText(WavyTextParams params) { return {pImpl->wavyText, params}; }

Program<Shaders::TitleTextParams> Shaders::titleText(TitleTextParams params) { return {pImpl->titleText, params}; }

Program<Shaders::ZXParams> Shaders::ZX(ZXParams params) { return {pImpl->zx, params}; }

BaseProgram Shaders::YCbCr() { return {pImpl->ycbcr}; }

Program<Shaders::ButtonParams> Shaders::button(ButtonParams params) { return {pImpl->button, params}; }

Program<Shaders::ArrowParams> Shaders::arrow(ArrowParams params) { return {pImpl->arrow, params}; }
