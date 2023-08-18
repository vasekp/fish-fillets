#ifndef FISH_FILLETS_GRAPHICS_BACKEND_FWD_H
#define FISH_FILLETS_GRAPHICS_BACKEND_FWD_H

#ifdef FISH_FILLETS_USE_VULKAN
#define BACKEND vulkan
#else
#define BACKEND ogl
#endif

namespace BACKEND {
    class Display;
    class Texture;
    class Program;
    class Framebuffer;
}

class GraphicsBackend;

#endif // FISH_FILLETS_GRAPHICS_BACKEND_FWD_H
