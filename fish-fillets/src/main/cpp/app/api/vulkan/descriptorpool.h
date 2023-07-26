#ifndef FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H
#define FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H

namespace vulkan {

class DescriptorPool {
    static constexpr std::size_t maxDescriptors = 500;

    vk::raii::DescriptorPool m_descriptorPool;
    vk::raii::DescriptorSetLayout m_descriptorSetLayout;
    vk::raii::DescriptorSets m_descriptorSets;

    std::vector<const vk::DescriptorSet*> m_freeSets;

public:
    DescriptorPool(const vk::raii::Device& device);

    const vk::DescriptorSetLayout& descriptorSetLayout() const;
    const vk::DescriptorSet* allocDescriptorSet();
    void freeDescriptorSet(const vk::DescriptorSet* descriptorSet);

private:
    vk::raii::DescriptorPool createDescriptorPool(const vk::raii::Device& device);
    vk::raii::DescriptorSetLayout createDescriptorSetLayout(const vk::raii::Device& device);
    vk::raii::DescriptorSets createDescriptorSets(const vk::raii::Device& device);
};

}

#endif //FISH_FILLETS_VULKAN_DESCRIPTOR_POOL_H
