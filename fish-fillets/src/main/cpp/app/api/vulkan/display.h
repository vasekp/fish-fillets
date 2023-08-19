#ifndef FISH_FILLETS_VULKAN_DISPLAY_H
#define FISH_FILLETS_VULKAN_DISPLAY_H

// always included from vulkan.h
#include "descriptorpool.h"

namespace vulkan {

class Display {
public:
    template<typename... NativeArgs>
    Display(const NativeArgs& ... nativeArgs) :
        m_context{},
        m_instance{createInstance()},
        m_messenger{createMessenger()},
        m_surface{createSurface(nativeArgs...)},
        m_physDevice{choosePhysicalDevice()},
        m_memoryProperties{m_physDevice.getMemoryProperties()},
        m_queueFamily{findQueueFamily(m_physDevice)},
        m_device{createLogicalDevice()},
        m_queue{m_device, m_queueFamily, 0},
        m_commandPool{createCommandPool()},
        m_commandBuffers{createCommandBuffers()},
        m_renderPass{createRenderPass()},
        m_descriptorPool{m_device},
        m_samplerLinear{createSampler(vk::Filter::eLinear)},
        m_samplerNearest{createSampler(vk::Filter::eNearest)},
        m_swapchainInfo{createSwapchainInfo()},
        m_swapchain{m_device, m_swapchainInfo},
        m_swapchainImages{m_swapchain.getImages()}
    { }

    Display(Display&&) = default;

    template<typename Target>
    vk::raii::DeviceMemory allocMemory(const Target& target, vk::MemoryPropertyFlags reqProperties) const;

    const auto& device() const { return m_device; }
    const auto& queue() const { return *m_queue; }
    const auto& commandBuffer() const { return *m_commandBuffers[0]; }
    const auto& renderPass() const { return *m_renderPass; }
    const auto& samplerLinear() const { return *m_samplerLinear; }
    const auto& samplerNearest() const { return *m_samplerNearest; }
    auto& descriptors() { return m_descriptorPool; }
    const auto& swapchain() const { return m_swapchain; }
    const auto& swapchainImages() const { return m_swapchainImages; }

    auto width() const { return m_swapchainInfo.imageExtent.width; }
    auto height() const { return m_swapchainInfo.imageExtent.height; }
    USize size() const { return {width(), height()}; }
    void notifyResize(USize sz);

private:
    vk::raii::Context m_context;
    vk::raii::Instance m_instance;
    std::optional<vk::raii::DebugUtilsMessengerEXT> m_messenger;
    vk::raii::SurfaceKHR m_surface;
    vk::raii::PhysicalDevice m_physDevice;
    vk::PhysicalDeviceMemoryProperties m_memoryProperties;
    std::uint32_t m_queueFamily;
    vk::raii::Device m_device;
    vk::raii::Queue m_queue;
    vk::raii::CommandPool m_commandPool;
    vk::raii::CommandBuffers m_commandBuffers;
    vk::raii::RenderPass m_renderPass;
    DescriptorPool m_descriptorPool;
    vk::raii::Sampler m_samplerLinear;
    vk::raii::Sampler m_samplerNearest;
    vk::SwapchainCreateInfoKHR m_swapchainInfo;
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::Image> m_swapchainImages;

    vk::raii::Instance createInstance();
    std::optional<vk::raii::DebugUtilsMessengerEXT> createMessenger();
    template<typename... NativeArgs>
    vk::raii::SurfaceKHR createSurface(const NativeArgs& ... nativeArgs);
    vk::raii::PhysicalDevice choosePhysicalDevice();
    std::uint32_t findQueueFamily(const vk::raii::PhysicalDevice& device);
    vk::raii::Device createLogicalDevice();
    vk::raii::CommandPool createCommandPool();
    vk::raii::CommandBuffers createCommandBuffers();
    vk::raii::RenderPass createRenderPass();
    vk::raii::Sampler createSampler(vk::Filter filter);
    vk::SwapchainCreateInfoKHR createSwapchainInfo(vk::SwapchainKHR old = nullptr);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*);

    static std::vector<const char*> extensions();
    static std::vector<const char*> platformExtensions();
};

}

#endif //FISH_FILLETS_VULKAN_DISPLAY_H
