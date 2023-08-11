#ifndef FISH_FILLETS_GRAPHICS_PLATFORM_FULL_H
#define FISH_FILLETS_GRAPHICS_PLATFORM_FULL_H

#include "platform-fwd.h"

#ifdef FISH_FILLETS_USE_VULKAN
#include "api/vulkan.h"
#else
#include "api/ogl.h"
#endif

#endif // FISH_FILLETS_GRAPHICS_PLATFORM_FULL_H
