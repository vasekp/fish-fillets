#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

class DrawTarget;

class BaseProgram {
protected:
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformType = vulkan::Program;
#else
    using PlatformType = ogl::Program;
#endif
    const PlatformType& m_native;
public:
    struct Params {
        FCoords src{};
        FCoords dest{};
        FCoords srcSize{};
        FCoords dstSize{};
        FCoords area{};
        Coords coords{};
        bool flipY{false};
    };

    enum class Shape {
        rect,
        triangle
    };

    using Textures = std::initializer_list<std::reference_wrapper<const Texture>>;

#ifdef FISH_FILLETS_USE_VULKAN
    struct BasePushConstants {
        std::array<float, 2> uSrcOffset;
        std::array<float, 2> uSrcSize;
        std::array<float, 2> uDstOffset;
        std::array<float, 2> uDstSize;
        std::array<float, 2> uArea;
        alignas(16) std::array<float, 4> uCoords;
    };

    constexpr static auto basePushConstantSize = sizeof(BasePushConstants);
    constexpr static auto ownPushConstantOffset = basePushConstantSize;
#endif

    BaseProgram(const PlatformType& native) : m_native(native) { }

    void run(GraphicsSystem& system, DrawTarget& target, const Params& params, Shape shape, const Textures& textures) const;

protected:
    virtual void own_params(GraphicsSystem& system) const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
    SpecParams m_params;

public:
    Program(const PlatformType& native, SpecParams params) : BaseProgram(native), m_params(params) { }

    SpecParams& params() { return m_params; }

private:
    void own_params(GraphicsSystem& system) const override;
};

class Shaders {
private:
#ifdef FISH_FILLETS_USE_VULKAN
    using PlatformShader = vulkan::Shader;
    using PlatformProgram = vulkan::Program;
#else
    using PlatformShader = ogl::Shader;
    using PlatformProgram = ogl::Program;
#endif
    PlatformShader m_vert;
    PlatformProgram m_copy;
    PlatformProgram m_maskCopy;
    PlatformProgram m_alpha;
    PlatformProgram m_reverse;
    PlatformProgram m_mirror;
    PlatformProgram m_flat;
    PlatformProgram m_blur;
    PlatformProgram m_disintegrate;
    PlatformProgram m_wavyImage;
    PlatformProgram m_wavyText;
    PlatformProgram m_titleText;
    PlatformProgram m_zx;
    PlatformProgram m_ycbcr;
    PlatformProgram m_button;
    PlatformProgram m_arrow;

public:
    Shaders(Instance& instance, GraphicsSystem& system);

    struct AlphaParams {
        float alpha = 1.f;
    };

    struct MaskCopyParams {
        Color maskColor;
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
    BaseProgram mirror() { return {m_mirror}; }
    Program<AlphaParams> alpha(AlphaParams params) { return {m_alpha, params}; }
    Program<FlatParams> flat(FlatParams params) { return {m_flat, params}; }
    Program<BlurParams> blur(BlurParams params) { return {m_blur, params}; }
    Program<DisintegrateParams> disintegrate(DisintegrateParams params) { return {m_disintegrate, params}; }
    Program<WavyImageParams> wavyImage(WavyImageParams params) { return {m_wavyImage, params}; }
    Program<WavyTextParams> wavyText(WavyTextParams params) { return {m_wavyText, params}; }
    Program<TitleTextParams> titleText(TitleTextParams params) { return {m_titleText, params}; }
    Program<ZXParams> ZX(ZXParams params) { return {m_zx, params}; }
    BaseProgram YCbCr() { return {m_ycbcr}; }
    Program<ButtonParams> button(ButtonParams params) { return {m_button, params}; }
    Program<ArrowParams> arrow(ArrowParams params) { return {m_arrow, params}; }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
