#ifndef FISH_FILLETS_VULKAN_PROGRAM_H
#define FISH_FILLETS_VULKAN_PROGRAM_H

namespace vulkan {

class Shader {
    std::string m_code;
    vk::raii::ShaderModule m_module;

public:
    Shader(Display& display, const std::string& code);

    operator const vk::ShaderModule&() const { return *m_module; }

private:
    std::vector<std::byte> fromString(const std::string& code);
};

class Program {
    vk::raii::PipelineLayout m_pipelineLayout;
    vk::raii::Pipeline m_pipeline;

public:
    Program(Display& display, const vk::ShaderModule& vertModule, const vk::ShaderModule& fragModule);

    operator const vk::Pipeline&() const { return *m_pipeline; }
    const vk::PipelineLayout& pipelineLayout() const { return *m_pipelineLayout; }

private:
    vk::raii::PipelineLayout createPipelineLayout(Display& display);
    vk::raii::Pipeline createPipeline(Display& display, const vk::ShaderModule& vertModule, const vk::ShaderModule& fragModule);
};

}

#endif // FISH_FILLETS_VULKAN_PROGRAM_H
