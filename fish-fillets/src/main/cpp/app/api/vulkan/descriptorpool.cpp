#include "../vulkan.h"

namespace vulkan {

DescriptorPool::DescriptorPool(const vk::raii::Device& device) :
    m_descriptorPool{createDescriptorPool(device)},
    m_descriptorSetLayout{createDescriptorSetLayout(device)},
    m_descriptorSets{createDescriptorSets(device)}
{
    m_freeSets.reserve(m_descriptorSets.size());
    for(const auto& descriptorSet : m_descriptorSets)
        m_freeSets.push_back(&*descriptorSet);
}

vk::raii::DescriptorPool DescriptorPool::createDescriptorPool(const vk::raii::Device& device) {
    auto poolSize = vk::DescriptorPoolSize{}
            .setType(vk::DescriptorType::eCombinedImageSampler)
            .setDescriptorCount(maxDescriptors);
    return {device, vk::DescriptorPoolCreateInfo{}
            .setPoolSizes(poolSize)
            .setMaxSets(maxDescriptors)
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

vk::raii::DescriptorSets DescriptorPool::createDescriptorSets(const vk::raii::Device& device) {
    std::array<vk::DescriptorSetLayout, maxDescriptors> layouts;
    std::fill(layouts.begin(), layouts.end(), *m_descriptorSetLayout);

    return {device, vk::DescriptorSetAllocateInfo{}
            .setDescriptorPool(*m_descriptorPool)
            .setSetLayouts(layouts)};
}

const vk::DescriptorSetLayout& DescriptorPool::descriptorSetLayout() const {
    return *m_descriptorSetLayout;
}

const vk::DescriptorSet* DescriptorPool::allocDescriptorSet() {
    Log::verbose<Log::graphics>("allocDescriptorSet: ", m_freeSets.size(), " - 1");
    const auto* ret = m_freeSets.back();
    m_freeSets.pop_back();
    return ret;
}

void DescriptorPool::freeDescriptorSet(const vk::DescriptorSet* descriptorSet) {
    Log::verbose<Log::graphics>("freeDescriptorSet: ", m_freeSets.size(), " + 1");
    m_freeSets.push_back(descriptorSet);
}

}
