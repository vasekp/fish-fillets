#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

class Shaders {
private:
#ifdef FISH_FILLETS_USE_VULKAN
    vulkan::Shader m_vert;
    vulkan::Program m_copy;
    vulkan::Program m_maskCopy;
    vulkan::Program m_alpha;
    vulkan::Program m_reverse;
    vulkan::Program m_mirror;
    vulkan::Program m_flat;
    vulkan::Program m_blur;
    vulkan::Program m_disintegrate;
    vulkan::Program m_wavyImage;
    vulkan::Program m_wavyText;
    vulkan::Program m_titleText;
    vulkan::Program m_zx;
    vulkan::Program m_ycbcr;
    vulkan::Program m_button;
    vulkan::Program m_arrow;
#else
    ogl::Program m_copy;
    ogl::Program m_alpha;
    ogl::Program m_maskCopy;
    ogl::Program m_reverse;
    ogl::Program m_mirror;
    ogl::Program m_flat;
    ogl::Program m_blur;
    ogl::Program m_disintegrate;
    ogl::Program m_wavyImage;
    ogl::Program m_wavyText;
    ogl::Program m_titleText;
    ogl::Program m_zx;
    ogl::Program m_ycbcr;
    ogl::Program m_button;
    ogl::Program m_arrow;
#endif

public:
    Shaders(Instance& instance, GraphicsSystem& system);

    struct AlphaParams {
        float alpha = 1.f;
    };

    struct MaskCopyParams {
        Color maskColor;
        const Texture& maskImage;
    };

    struct MirrorParams {
        const Texture& maskImage;
    };

    struct FlatParams {
        Color color;
        float alpha = 1.f;
    };

    struct BlurParams {
        FCoords dir;
    };

    struct DisintegrateParams {
        float time;
    };

    struct WavyImageParams {
        float amplitude;
        float period;
        float speed;
        float phase;
    };

    struct WavyTextParams {
        Color color1;
        Color color2;
        float time;
    };

    struct TitleTextParams {
        FCoords blitSize;
        Color color;
        float alpha;
    };

    struct ZXParams {
        Color color1;
        Color color2;
        float period;
        float offset;
    };

    struct YCbCrParams {
        const Texture& texY;
        const Texture& texCb;
        const Texture& texCr;
    };

    struct ButtonParams {
        FCoords texSize;
        Color color;
        float alpha;
    };

    struct ArrowParams {
        FCoords position;
        float size;
        FCoords direction = {};
        bool inwards;
        Color color;
        float alpha = 1.f;
    };

    BaseProgram copy() { return {m_copy}; }
    Program<MaskCopyParams> maskCopy(MaskCopyParams params) { return {m_maskCopy, params}; }
    BaseProgram reverse() { return {m_reverse}; }
    Program<MirrorParams> mirror(MirrorParams params) { return {m_mirror, params}; }
    Program<AlphaParams> alpha(AlphaParams params) { return {m_alpha, params}; }
    Program<FlatParams> flat(FlatParams params) { return {m_flat, params}; }
    Program<BlurParams> blur(BlurParams params) { return {m_blur, params}; }
    Program<DisintegrateParams> disintegrate(DisintegrateParams params) { return {m_disintegrate, params}; }
    Program<WavyImageParams> wavyImage(WavyImageParams params) { return {m_wavyImage, params}; }
    Program<WavyTextParams> wavyText(WavyTextParams params) { return {m_wavyText, params}; }
    Program<TitleTextParams> titleText(TitleTextParams params) { return {m_titleText, params}; }
    Program<ZXParams> ZX(ZXParams params) { return {m_zx, params}; }
    Program<YCbCrParams> YCbCr(YCbCrParams params) { return {m_ycbcr, params}; }
    Program<ButtonParams> button(ButtonParams params) { return {m_button, params}; }
    Program<ArrowParams> arrow(ArrowParams params) { return {m_arrow, params}; }

private:
    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texMask_shader = 1;
    constexpr static GLint texCb_shader = 1;
    constexpr static GLint texCr_shader = 2;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texMask_gl = GL_TEXTURE1;
    constexpr static GLint texCb_gl = GL_TEXTURE1;
    constexpr static GLint texCr_gl = GL_TEXTURE2;

    template<typename SpecParams>
    friend class Program;
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
