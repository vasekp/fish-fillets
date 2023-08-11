#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

class DrawTarget;
class GraphicsSystem;

class BaseProgram {
protected:
    const Platform::Program& m_native;
public:
    struct Params {
        vec2 src{};
        vec2 srcSize{};
        vec2 dest{};
        vec2 dstSize{};
        vec2 area{};
        vec4 coords{};
    };

    enum class Shape {
        rect,
        triangle
    };

    using Textures = std::initializer_list<std::reference_wrapper<const Texture>>;

    BaseProgram(const Platform::Program& native) : m_native(native) { }

    void run(GraphicsSystem& system, DrawTarget& target, const Params& params, Shape shape, const Textures& textures) const;

protected:
    virtual void own_params(GraphicsSystem& system) const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
    SpecParams m_params;

public:
    Program(const Platform::Program& native, SpecParams params) : BaseProgram(native), m_params(params) { }

    SpecParams& params() { return m_params; }

private:
    void own_params(GraphicsSystem& system) const override;
};

class Shaders {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

public:
    Shaders(Instance& instance, GraphicsSystem& system);
    ~Shaders();

    struct AlphaParams {
        float alpha = 1.f;
    };

    struct MaskCopyParams {
        vec4 maskColor;
    };

    struct FlatParams {
        vec4 color;
    };

    struct BlurParams {
        vec2 dir;
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
        vec4 color1;
        vec4 color2;
        float time;
    };

    struct TitleTextParams {
        vec4 color;
    };

    struct ZXParams {
        vec4 color1;
        vec4 color2;
        float period;
        float offset;
    };

    struct ButtonParams {
        vec4 color;
    };

    struct ArrowParams {
        vec2 position;
        vec2 direction = {};
        float size;
        float sign;
        vec4 color;
    };

    BaseProgram copy();
    Program<MaskCopyParams> maskCopy(MaskCopyParams params);
    BaseProgram reverse();
    BaseProgram mirror();
    Program<AlphaParams> alpha(AlphaParams params);
    Program<FlatParams> flat(FlatParams params);
    Program<BlurParams> blur(BlurParams params);
    Program<DisintegrateParams> disintegrate(DisintegrateParams params);
    Program<WavyImageParams> wavyImage(WavyImageParams params);
    Program<WavyTextParams> wavyText(WavyTextParams params);
    Program<TitleTextParams> titleText(TitleTextParams params);
    Program<ZXParams> ZX(ZXParams params);
    BaseProgram YCbCr();
    Program<ButtonParams> button(ButtonParams params);
    Program<ArrowParams> arrow(ArrowParams params);
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
