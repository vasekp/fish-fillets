#ifndef FISH_FILLETS_GRAPHICS_PLATFORM_FWD_H
#define FISH_FILLETS_GRAPHICS_PLATFORM_FWD_H

#ifdef FISH_FILLETS_USE_VULKAN
#define Platform vulkan
#else
#define Platform ogl
#endif

namespace Platform {
    class Display;
    class Texture;
    class Program;
}

#endif // FISH_FILLETS_GRAPHICS_PLATFORM_FWD_H
