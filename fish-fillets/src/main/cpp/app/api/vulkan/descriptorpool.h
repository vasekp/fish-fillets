#ifndef FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H
#define FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H

// always included from vulkan.h

namespace vulkan {

class DescriptorPool {
public:
    DescriptorPool(const vk::raii::Device& device);

    const vk::DescriptorSetLayout& descriptorSetLayout() const;
    const vk::DescriptorSet* allocDescriptorSet(int binding);
    void freeDescriptorSet(int binding, const vk::DescriptorSet* descriptorSet);

private:
    vk::raii::DescriptorPool m_descriptorPool;
    vk::raii::DescriptorSetLayout m_descriptorSetLayout;
    std::array<vk::raii::DescriptorSets, 3> m_descriptorSets;

    std::array<std::vector<const vk::DescriptorSet*>, 3> m_freeSets;

    vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device& device);
    vk::raii::DescriptorSetLayout createDescriptorSetLayout(const vk::raii::Device& device);
    vk::raii::DescriptorSets createDescriptorSets(const vk::raii::Device& device, int binding);
};

}

#endif //FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H
