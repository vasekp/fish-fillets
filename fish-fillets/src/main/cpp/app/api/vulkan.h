#ifndef FISH_FILLETS_VULKAN_H
#define FISH_FILLETS_VULKAN_H

#include "common.h"

#include <vulkan/vulkan_raii.hpp>

namespace vulkan {
    constexpr bool useValidation = false;

    constexpr auto baseRange = vk::ImageSubresourceRange{}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setBaseMipLevel(0)
        .setLevelCount(1)
        .setBaseArrayLayer(0)
        .setLayerCount(1);

    constexpr auto baseLayers = vk::ImageSubresourceLayers{}
        .setAspectMask(vk::ImageAspectFlagBits::eColor)
        .setMipLevel(0)
        .setBaseArrayLayer(0)
        .setLayerCount(1);
}

#include "vulkan/display.h"
#include "vulkan/descriptorpool.h"
#include "vulkan/texture.h"
#include "vulkan/framebuffer.h"
#include "vulkan/program.h"

#endif //FISH_FILLETS_VULKAN_H
