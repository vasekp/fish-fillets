#ifndef FISH_FILLETS_VULKAN_DISPLAY_H
#define FISH_FILLETS_VULKAN_DISPLAY_H

namespace vulkan {

class Display {
    vk::raii::Context m_context;
    vk::raii::Instance m_instance;
    std::optional<vk::raii::DebugUtilsMessengerEXT> m_messenger;
    vk::raii::SurfaceKHR m_surface;
    vk::raii::PhysicalDevice m_physDevice;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;
    std::uint32_t m_queueFamily;
    vk::raii::Device m_device;
    vk::raii::Queue m_queue;
    // TODO cut here, move to another class
    vk::raii::CommandPool m_commandPool;
    vk::raii::CommandBuffers m_commandBuffers;
    const vk::CommandBuffer* m_commandBuffer; // TODO ref
    vk::raii::DescriptorPool m_descriptorPool;
    vk::raii::DescriptorSetLayout m_descriptorSetLayout;
    vk::raii::DescriptorSets m_descriptorSets;
    vk::raii::RenderPass m_renderPass;
    vk::raii::Sampler m_samplerLinear;
    // TODO cut here, move to another class
    vk::SwapchainCreateInfoKHR m_swapchainInfo;
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::Image> m_swapchainImages;

    constexpr static std::size_t maxDescriptors = 500;
    mutable std::bitset<maxDescriptors> m_usedDescriptors{};
    mutable std::size_t m_lastDescriptorIndex = 0;

public:
    template<typename... NativeArgs>
    Display(const NativeArgs& ... nativeArgs) :
        m_context{},
        m_instance{createInstance()},
        m_messenger{useValidation ? createMessenger() : decltype(m_messenger){}},
        m_surface{createSurface(nativeArgs...)},
        m_physDevice{choosePhysicalDevice()},
        m_memoryProperties{m_physDevice.getMemoryProperties()},
        m_queueFamily{findQueueFamily(m_physDevice)},
        m_device{createLogicalDevice()},
        m_queue{m_device, m_queueFamily, 0},
        m_commandPool{createCommandPool()},
        m_commandBuffers{createCommandBuffers()},
        m_commandBuffer{&*m_commandBuffers[0]},
        m_descriptorPool{createDescriptorPool()},
        m_descriptorSetLayout{createDescriptorSetLayout()},
        m_descriptorSets{createDescriptorSets()},
        m_renderPass{createRenderPass()},
        m_samplerLinear{createSamplerLinear()},
        m_swapchainInfo{createSwapchainInfo()},
        m_swapchain{m_device, m_swapchainInfo},
        m_swapchainImages{m_swapchain.getImages()}
    { }

    Display(Display&&) = default;

    template<typename Target>
    vk::raii::DeviceMemory allocMemory(const Target& target, vk::MemoryPropertyFlags reqProperties) const;

    void recreateSwapchain();

    const auto& device() const { return m_device; }
    const auto& queue() const { return *m_queue; }
    const auto& commandBuffer() const { return *m_commandBuffer; }
    const auto& renderPass() const { return *m_renderPass; }
    const auto& samplerLinear() const { return *m_samplerLinear; }
    const auto& descriptorSetLayout() const { return *m_descriptorSetLayout; }
    const auto& swapchain() const { return m_swapchain; }
    const auto& swapchainImages() const { return m_swapchainImages; }

    std::size_t allocDescriptorSet() const;
    void freeDescriptorSet(std::size_t index) const;
    const vk::DescriptorSet& getDescriptorSet(std::size_t index) const { return *m_descriptorSets[index]; }

    auto width() const { return m_swapchainInfo.imageExtent.width; }
    auto height() const { return m_swapchainInfo.imageExtent.height; }
    ICoords size() const { return {(int)width(), (int)height()}; }

private:
    vk::raii::Instance createInstance();
    vk::raii::DebugUtilsMessengerEXT createMessenger();
    template<typename... NativeArgs>
    vk::raii::SurfaceKHR createSurface(const NativeArgs& ... nativeArgs);
    vk::raii::PhysicalDevice choosePhysicalDevice();
    std::uint32_t findQueueFamily(const vk::raii::PhysicalDevice& device);
    vk::raii::Device createLogicalDevice();
    vk::raii::CommandPool createCommandPool();
    vk::raii::CommandBuffers createCommandBuffers();
    vk::raii::DescriptorPool createDescriptorPool();
    vk::raii::DescriptorSetLayout createDescriptorSetLayout();
    vk::raii::DescriptorSets createDescriptorSets();
    vk::raii::RenderPass createRenderPass();
    vk::raii::Sampler createSamplerLinear();
    vk::SwapchainCreateInfoKHR createSwapchainInfo(vk::SwapchainKHR old = nullptr);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

    constexpr static auto debugUtilsInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            /* | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral*/
            /* | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance*/)
        .setPfnUserCallback(&debugCallback);

    static std::vector<const char*> extensions();
    static std::vector<const char*> platformExtensions();
};

}

#endif //FISH_FILLETS_VULKAN_DISPLAY_H
