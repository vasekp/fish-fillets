#include "api/vulkan.h"
#include <X11/Xlib.h>

namespace vulkan {

std::vector<const char*> Display::platformExtensions() {
    return {VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
}

using PDisplay = ::Display*;

template<>
vk::raii::SurfaceKHR Display::createSurface(const PDisplay& dpy, const Window& win) {
    return m_instance.createXlibSurfaceKHR(vk::XlibSurfaceCreateInfoKHR{{}, dpy, win});
}

}
