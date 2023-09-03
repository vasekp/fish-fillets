#include "api/vulkan.h"

namespace vulkan {

std::vector<const char*> Display::platformExtensions() {
    return {VK_EXT_METAL_SURFACE_EXTENSION_NAME};
}

using PLayer = CAMetalLayer*;

template<>
vk::raii::SurfaceKHR Display::createSurface(const PLayer& pView) {
    return m_instance.createMetalSurfaceEXT(vk::MetalSurfaceCreateInfoEXT{{}, pView});
}

}
