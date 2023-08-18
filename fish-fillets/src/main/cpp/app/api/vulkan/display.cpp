#include "api/vulkan.h"

namespace vulkan {

static constexpr bool useValidation = true;

namespace {
    constexpr std::uint32_t noFamily = std::numeric_limits<std::uint32_t>::max();

    static constexpr auto debugUtilsInfo = vk::DebugUtilsMessengerCreateInfoEXT{}
        .setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
        .setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            /* | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral*/
            /* | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance*/)
        .setPfnUserCallback(&debugCallback);

    template<typename T>
    const char* name(const T&);

    template<>
    const char* name(const vk::ExtensionProperties& ext) { return ext.extensionName; }

    template<>
    const char* name(const vk::LayerProperties& ext) { return ext.layerName; }

    template<typename T>
    bool contains(const std::vector<T>& vector, const std::string& string) {
        return std::find_if(vector.begin(), vector.end(), [&string](const T& entry) { return name(entry) == string; }) != vector.end();
    }
}

static const std::vector<const char*> layers = useValidation
        ? std::vector<const char*>{"VK_LAYER_KHRONOS_validation"}
        : std::vector<const char*>{};

vk::raii::Instance Display::createInstance() {
    for(const auto& layerName : layers)
        if(!contains(m_context.enumerateInstanceLayerProperties(), layerName))
            Log::fatal("Validation layer "s + layerName + " required but not supported.");

    auto extensions = platformExtensions();
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    if constexpr(useValidation)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    vk::StructureChain<vk::InstanceCreateInfo, vk::DebugUtilsMessengerCreateInfoEXT> instanceInfo{
            vk::InstanceCreateInfo{}
                    .setPEnabledLayerNames(layers)
                    .setPEnabledExtensionNames(extensions),
            debugUtilsInfo};
    if constexpr(!useValidation)
        instanceInfo.unlink<std::remove_const_t<decltype(debugUtilsInfo)>>();
    return {m_context, instanceInfo.get<vk::InstanceCreateInfo>()};
}

std::optional<vk::raii::DebugUtilsMessengerEXT> Display::createMessenger() {
    return useValidation
        ? m_instance.createDebugUtilsMessengerEXT(debugUtilsInfo)
        : std::optional<vk::raii::DebugUtilsMessengerEXT>{};
}

vk::raii::PhysicalDevice Display::choosePhysicalDevice() {
    auto rank = [](vk::PhysicalDeviceType type) {
        switch(type) {
            case vk::PhysicalDeviceType::eDiscreteGpu:
                return 1;
            case vk::PhysicalDeviceType::eIntegratedGpu:
                return 2;
            case vk::PhysicalDeviceType::eVirtualGpu:
                return 3;
            case vk::PhysicalDeviceType::eCpu:
                return 4;
            default:
                return 5;
        }
    };
    auto devices = vk::raii::PhysicalDevices{m_instance};
    std::sort(devices.begin(), devices.end(), [rank](const auto& one, const auto& two) {
        return rank(one.getProperties().deviceType) < rank(two.getProperties().deviceType);
    });

    for(const auto& device : devices) {
        Log::debug<Log::graphics>("Vulkan device type: ", vk::to_string(device.getProperties().deviceType));

        if(!contains(device.enumerateDeviceExtensionProperties(), VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            Log::debug<Log::graphics>("Skipping: does not support swapchain");
            continue;
        }

        auto surfaceFormats = device.getSurfaceFormatsKHR(*m_surface);
        auto surfacePresentModes = device.getSurfacePresentModesKHR(*m_surface);
        if(surfaceFormats.empty() || surfacePresentModes.empty()) {
            Log::debug<Log::graphics>("Skipping: does not support surface present modes");
            continue;
        } else {
            for(const auto& format : surfaceFormats)
                Log::debug<Log::graphics>("Surface format: ", vk::to_string(format.format), ' ', vk::to_string(format.colorSpace));
        }

#if 0
        std::optional<std::uint32_t> graphicsFamily, presentFamily;
#endif
        auto families = device.getQueueFamilyProperties();
        for(auto i = 0u; i < families.size(); i++)
            Log::debug<Log::graphics>("Family ", i, ": ", vk::to_string(families[i].queueFlags));
#if 0
        for(auto i = 0u; i < families.size(); i++) {
            const auto& family = families[i];
            bool supportsPresent = device.getSurfaceSupportKHR(i, *m_surface);
            bool supportsGraphics = bool(family.queueFlags & vk::QueueFlagBits::eGraphics);
            if(supportsPresent && supportsGraphics) {
                graphicsFamily = i;
                presentFamily = i;
                break;
            } else if(supportsGraphics && !graphicsFamily.has_value())
                graphicsFamily = i;
            else if(supportsPresent && !presentFamily.has_value())
                presentFamily = i;
        }
        if(graphicsFamily.has_value() && presentFamily.has_value())
            return {device, QueueFamilyIndices{graphicsFamily.value(), presentFamily.value()}};
#endif
        if(findQueueFamily(device) != noFamily) {
            Log::info<Log::graphics>("Using Vulkan device type: ", vk::to_string(device.getProperties().deviceType));
            return device;
        }
    }
    // none found
    Log::fatal("No device matches criteria.");
}

std::uint32_t Display::findQueueFamily(const vk::raii::PhysicalDevice& device) {
    auto families = device.getQueueFamilyProperties();
    for(auto i = 0u; i < families.size(); i++) {
        const auto& family = families[i];
        bool supportsPresent = device.getSurfaceSupportKHR(i, *m_surface);
        bool supportsGraphics = bool(family.queueFlags & vk::QueueFlagBits::eGraphics);
        if(supportsPresent && supportsGraphics)
            return i;
    }
    return noFamily;
}

vk::raii::Device Display::createLogicalDevice() {
    float priority = 1.f;
    auto queueInfo = vk::DeviceQueueCreateInfo{}
            .setQueueFamilyIndex(m_queueFamily)
            .setQueueCount(1)
            .setPQueuePriorities(&priority);
#if 0
    std::vector<std::uint32_t> uniqueFamilies = {queueFamilies.graphics, queueFamilies.presentation};
    std::sort(uniqueFamilies.begin(), uniqueFamilies.end());
    uniqueFamilies.erase(std::unique(uniqueFamilies.begin(), uniqueFamilies.end()), uniqueFamilies.end());
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(uniqueFamilies.size());
    for(auto family : uniqueFamilies)
        queueCreateInfos.push_back({{}, family, 1, &priority});
#endif
    vk::PhysicalDeviceFeatures features{};
    std::vector<const char*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    return {m_physDevice, vk::DeviceCreateInfo{}
            .setQueueCreateInfos(queueInfo)
            .setPEnabledLayerNames(layers)
            .setPEnabledFeatures(&features)
            .setPEnabledExtensionNames(deviceExtensions)
    };
}

vk::raii::CommandPool Display::createCommandPool() {
    return {m_device, vk::CommandPoolCreateInfo{}
            .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
            .setQueueFamilyIndex(m_queueFamily)};
}

vk::raii::CommandBuffers Display::createCommandBuffers() {
    return {m_device, vk::CommandBufferAllocateInfo{}
            .setCommandPool(*m_commandPool)
            .setCommandBufferCount(1)
            .setLevel(vk::CommandBufferLevel::ePrimary)};
}

vk::raii::RenderPass Display::createRenderPass() {
    auto colorAttachment = vk::AttachmentDescription{}
            .setFormat(vk::Format::eR8G8B8A8Unorm)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eLoad)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    auto colorAttachmentRef = vk::AttachmentReference{}
            .setAttachment(0)
            .setLayout(vk::ImageLayout::eColorAttachmentOptimal);
    auto dependency = vk::SubpassDependency{}
            .setSrcSubpass(vk::SubpassExternal)
            .setDstSubpass(0)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead);
    auto subpass = vk::SubpassDescription{}
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachments(colorAttachmentRef);
    return {m_device, vk::RenderPassCreateInfo{}
            .setAttachments(colorAttachment)
            .setSubpasses(subpass)
            .setDependencies(dependency)};
}

vk::raii::Sampler Display::createSampler(vk::Filter filter) {
    return {m_device, vk::SamplerCreateInfo{}
            .setMagFilter(filter)
            .setMinFilter(filter)
            .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
            .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
            .setAnisotropyEnable(vk::False)
            .setUnnormalizedCoordinates(vk::False)
            .setCompareEnable(vk::False)};
}

vk::SwapchainCreateInfoKHR Display::createSwapchainInfo(vk::SwapchainKHR old) {
    auto surfaceCapabilities = m_physDevice.getSurfaceCapabilitiesKHR(*m_surface);
    auto surfaceFormats = m_physDevice.getSurfaceFormatsKHR(*m_surface);

    Log::debug<Log::graphics>("Creating swapchain for size ", surfaceCapabilities.currentExtent.width, 'x', surfaceCapabilities.currentExtent.height);

    auto swapchainInfo = vk::SwapchainCreateInfoKHR{}
            .setSurface(*m_surface)
            .setImageFormat(surfaceFormats[0].format)
            .setImageColorSpace(surfaceFormats[0].colorSpace)
            .setImageExtent({surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height})
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst)
            .setPresentMode(vk::PresentModeKHR::eFifo)
            .setPreTransform(surfaceCapabilities.currentTransform)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setClipped(true)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setOldSwapchain(old);

    if(surfaceCapabilities.maxImageCount == 0)
        swapchainInfo.setMinImageCount(surfaceCapabilities.minImageCount + 1);
    else
        swapchainInfo.setMinImageCount(std::min(surfaceCapabilities.minImageCount + 1, surfaceCapabilities.maxImageCount));

    Log::debug<Log::graphics>("Surface usage flags: ", vk::to_string(surfaceCapabilities.supportedUsageFlags));

    Log::debug<Log::graphics>("Framebuffer extent: ", swapchainInfo.imageExtent.width, "×", swapchainInfo.imageExtent.height, ", format: ", vk::to_string(swapchainInfo.imageFormat));

    for(const auto& format : surfaceFormats)
        if(format.format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            swapchainInfo.setImageFormat(format.format).setImageColorSpace(format.colorSpace);
            break;
        }
    return swapchainInfo;
}

void Display::recreateSwapchain() {
    m_swapchainInfo = createSwapchainInfo(*m_swapchain);
    m_swapchain = vk::raii::SwapchainKHR{m_device, m_swapchainInfo};
    m_swapchainImages = m_swapchain.getImages();
}

template<typename Target>
vk::raii::DeviceMemory Display::allocMemory(const Target& target, vk::MemoryPropertyFlags reqProps) const {
    auto memoryReqs = target.getMemoryRequirements();
    auto memTypeIndex = [this, &memoryReqs, reqProps]() {
        for(auto i = 0u; i < m_memoryProperties.memoryTypeCount; i++)
            if((memoryReqs.memoryTypeBits & (1 << i)) && ((m_memoryProperties.memoryTypes[i].propertyFlags & reqProps) == reqProps))
                return i;
        // not found
        Log::fatal("No suitable memory type found.");
    }();
    vk::raii::DeviceMemory memory{m_device, vk::MemoryAllocateInfo{}
            .setAllocationSize(memoryReqs.size)
            .setMemoryTypeIndex(memTypeIndex)};
    target.bindMemory(*memory, 0);
    return memory;
}

template vk::raii::DeviceMemory Display::allocMemory(const vk::raii::Buffer&, vk::MemoryPropertyFlags) const;
template vk::raii::DeviceMemory Display::allocMemory(const vk::raii::Image&, vk::MemoryPropertyFlags) const;

VKAPI_ATTR VkBool32 VKAPI_CALL Display::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
    auto typeString = vk::to_string(vk::DebugUtilsMessageTypeFlagsEXT(messageType));
    switch(messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            Log::verbose<Log::graphics>(typeString, ' ', pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            Log::info<Log::graphics>(typeString, ' ', pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            Log::warn(typeString, ' ', pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            Log::error(typeString, ' ', pCallbackData->pMessage);
            break;
        default: {
            auto severityString = vk::to_string(vk::DebugUtilsMessageSeverityFlagBitsEXT(messageSeverity));
            Log::warn(severityString, ": ", typeString, ' ', pCallbackData->pMessage);
            break;
        }
    }
    return false;
}

}
