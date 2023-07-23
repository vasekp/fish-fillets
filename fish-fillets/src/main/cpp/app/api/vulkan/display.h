#ifndef FISH_FILLETS_VULKAN_DISPLAY_H
#define FISH_FILLETS_VULKAN_DISPLAY_H

namespace vulkan {

class Display {
    const vk::raii::Context m_context;
    const vk::raii::Instance m_instance;
    const std::optional<vk::raii::DebugUtilsMessengerEXT> m_messenger;
    const vk::raii::SurfaceKHR m_surface;
    const vk::raii::PhysicalDevice m_physDevice;
    const vk::PhysicalDeviceMemoryProperties m_memoryProperties;
    const std::uint32_t m_queueFamily;
    const vk::raii::Device m_device;
    const vk::raii::Queue m_queue;
    const vk::raii::CommandPool m_commandPool;
    const vk::raii::CommandBuffers m_commandBuffers;
    const vk::CommandBuffer& m_commandBuffer;
    const vk::raii::RenderPass m_renderPass;
    vk::SwapchainCreateInfoKHR m_swapchainInfo;
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::Image> m_swapchainImages;

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
        m_commandBuffer{*m_commandBuffers[0]},
        m_renderPass{createRenderPass()},
        m_swapchainInfo{createSwapchainInfo()},
        m_swapchain{m_device, m_swapchainInfo},
        m_swapchainImages{m_swapchain.getImages()}
    { }

    template<typename Target>
    vk::raii::DeviceMemory allocMemory(const Target& target, vk::MemoryPropertyFlags reqProperties) const;

    void recreateSwapchain();

    const auto& device() const { return m_device; }
    const auto& queue() const { return *m_queue; }
    const auto& commandBuffer() const { return m_commandBuffer; }
    const auto& renderPass() const { return *m_renderPass; }
    const auto& swapchain() const { return m_swapchain; }
    const auto& swapchainImages() const { return m_swapchainImages; }

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
    vk::raii::RenderPass createRenderPass();
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
