#include "../vulkan.h"

// TODO
    struct PushConstants {
        std::array<float, 2> uSrcOffset;
        std::array<float, 2> uSrcSize;
        std::array<float, 2> uDstOffset;
        std::array<float, 2> uDstSize;
        std::array<float, 2> uArea;
        alignas(16) std::array<float, 3> uCoords;
        alignas(16) std::array<float, 4> uColor; // TODO flat
    };

namespace vulkan {

Shader::Shader(Display& display, const std::string& code) :
    m_code(code),
    m_module{display.device(), vk::ShaderModuleCreateInfo{}
        .setCodeSize(m_code.size())
        .setPCode(reinterpret_cast<const std::uint32_t*>(m_code.data()))}
{ }


Program::Program(Display& display, const vk::ShaderModule& vertModule, const vk::ShaderModule& fragModule) :
    m_pipelineLayout(createPipelineLayout(display)),
    m_pipeline(createPipeline(display, vertModule, fragModule))
{ }

vk::raii::PipelineLayout Program::createPipelineLayout(Display& display) {
    auto pushConstantRange = vk::PushConstantRange{}
            .setSize(sizeof(PushConstants))
            .setStageFlags(vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment);
    return {display.device(), vk::PipelineLayoutCreateInfo{}
            .setSetLayouts(display.descriptors().descriptorSetLayout()) // TODO
            .setPushConstantRanges(pushConstantRange)};
}

vk::raii::Pipeline Program::createPipeline(Display& display, const vk::ShaderModule& vertModule, const vk::ShaderModule& fragModule) {
    static constexpr auto vertexInputInfo = vk::PipelineVertexInputStateCreateInfo{};
    static constexpr auto inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo{}
            .setTopology(vk::PrimitiveTopology::eTriangleStrip);
    static constexpr auto dynamicStates = std::array{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    static constexpr auto dynamicInfo = vk::PipelineDynamicStateCreateInfo{}
            .setDynamicStateCount(2)
            .setPDynamicStates(dynamicStates.data());
    static constexpr auto rasterizationInfo = vk::PipelineRasterizationStateCreateInfo{}
            .setPolygonMode(vk::PolygonMode::eFill)
            .setCullMode(vk::CullModeFlagBits::eNone)
            .setLineWidth(1.f);
    static constexpr auto multisampleInfo = vk::PipelineMultisampleStateCreateInfo{}
            .setSampleShadingEnable(false)
            .setRasterizationSamples(vk::SampleCountFlagBits::e1);
    static constexpr auto colorBlendAttachment = vk::PipelineColorBlendAttachmentState{}
            .setColorWriteMask(
                    vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                    vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
            .setBlendEnable(true)
            .setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
            .setDstAlphaBlendFactor(vk::BlendFactor::eOne)
            .setSrcColorBlendFactor(vk::BlendFactor::eOne)
            .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(vk::BlendOp::eAdd);
    static constexpr auto colorblendInfo = vk::PipelineColorBlendStateCreateInfo{}
            .setAttachmentCount(1)
            .setPAttachments(&colorBlendAttachment);
    static constexpr vk::Viewport viewport{0.f, 0.f, 1.f, 1.f, 0.f, 1.f}; // dummy
    static constexpr vk::Rect2D scissor{{}, {}}; // dummy
    static constexpr auto viewportInfo = vk::PipelineViewportStateCreateInfo{}
            .setViewportCount(1)
            .setPViewports(&viewport)
            .setScissorCount(1)
            .setPScissors(&scissor);

    vk::PipelineShaderStageCreateInfo shaderStages[2]{
            vk::PipelineShaderStageCreateInfo{}
                    .setModule(vertModule)
                    .setStage(vk::ShaderStageFlagBits::eVertex)
                    .setPName("main"),
            vk::PipelineShaderStageCreateInfo{}
                    .setModule(fragModule)
                    .setStage(vk::ShaderStageFlagBits::eFragment)
                    .setPName("main")};

    return vk::raii::Pipeline{display.device(), nullptr, vk::GraphicsPipelineCreateInfo{}
            .setStages(shaderStages)
            .setPVertexInputState(&vertexInputInfo)
            .setPInputAssemblyState(&inputAssemblyInfo)
            .setPViewportState(&viewportInfo)
            .setPDynamicState(&dynamicInfo)
            .setPRasterizationState(&rasterizationInfo)
            .setPMultisampleState(&multisampleInfo)
            .setPColorBlendState(&colorblendInfo)
            .setLayout(*m_pipelineLayout)
            .setRenderPass(display.renderPass())
            .setSubpass(0)};
}

}
