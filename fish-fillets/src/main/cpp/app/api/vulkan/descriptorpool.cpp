#include "../vulkan.h"

namespace vulkan {

static constexpr std::array<std::size_t, 3> maxDescriptors = {500, 2, 1};

DescriptorPool::DescriptorPool(const vk::raii::Device& device) :
    m_descriptorPool{createDescriptorPool(device)},
    m_descriptorSetLayout{createDescriptorSetLayout(device)},
    m_descriptorSets{{
        createDescriptorSets(device, 0),
        createDescriptorSets(device, 1),
        createDescriptorSets(device, 2)}}
{
    for(auto binding = 0u; binding < 3; binding++) {
        m_freeSets[binding].reserve(m_descriptorSets[binding].size());
        for(const auto& descriptorSet : m_descriptorSets[binding])
            m_freeSets[binding].push_back(&*descriptorSet);
    }
}

vk::raii::DescriptorPool DescriptorPool::createDescriptorPool(const vk::raii::Device& device) {
    std::array<vk::DescriptorPoolSize, 3> poolSizes;
    std::size_t total = 0;
    for(int binding = 0; binding < 3; binding++) {
        poolSizes[binding] = vk::DescriptorPoolSize{}
            .setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(maxDescriptors[binding]);
        total += maxDescriptors[binding];
    }
    return {device, vk::DescriptorPoolCreateInfo{}
            .setPoolSizes(poolSizes)
            .setMaxSets(total)
            .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet)};
}

vk::raii::DescriptorSetLayout DescriptorPool::createDescriptorSetLayout(const vk::raii::Device& device) {
    auto setLayoutBinding = vk::DescriptorSetLayoutBinding{}
        .setBinding(0)
        .setDescriptorCount(1)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setStageFlags(vk::ShaderStageFlagBits::eFragment);

    return {device, vk::DescriptorSetLayoutCreateInfo{}.setBindings(setLayoutBinding)};
}

vk::raii::DescriptorSets DescriptorPool::createDescriptorSets(const vk::raii::Device& device, int binding) {
    std::vector<vk::DescriptorSetLayout> layouts;
    layouts.resize(maxDescriptors[binding]);
    std::fill(layouts.begin(), layouts.end(), *m_descriptorSetLayout);

    return {device, vk::DescriptorSetAllocateInfo{}
            .setDescriptorPool(*m_descriptorPool)
            .setSetLayouts(layouts)};
}

const vk::DescriptorSetLayout& DescriptorPool::descriptorSetLayout() const {
    return *m_descriptorSetLayout;
}

const vk::DescriptorSet* DescriptorPool::allocDescriptorSet(int binding) {
    Log::verbose<Log::graphics>("allocDescriptorSet [", binding, "]: ", m_freeSets[binding].size(), " - 1");
    const auto* ret = m_freeSets[binding].back();
    m_freeSets[binding].pop_back();
    return ret;
}

void DescriptorPool::freeDescriptorSet(int binding, const vk::DescriptorSet* descriptorSet) {
    Log::verbose<Log::graphics>("freeDescriptorSet [", binding, "]: ", m_freeSets[binding].size(), " + 1");
    m_freeSets[binding].push_back(descriptorSet);
}

}
