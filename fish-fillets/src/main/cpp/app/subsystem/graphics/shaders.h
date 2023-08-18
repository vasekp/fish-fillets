#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

// always included from graphics.h

class DrawTarget;
class GraphicsBackend;

class BaseProgram {
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

    BaseProgram(const BACKEND::Program& native) : m_native(native) { }

    void run(GraphicsBackend& backend, DrawTarget& target, const Params& params, Shape shape, const Textures& textures) const;

protected:
    const BACKEND::Program& m_native;

    virtual void own_params(GraphicsBackend& backend) const { }
};

template<typename SpecParams>
class Program : public BaseProgram {
public:
    Program(const BACKEND::Program& native, SpecParams params) : BaseProgram(native), m_params(params) { }

    SpecParams& params() { return m_params; }

private:
    SpecParams m_params;

    void own_params(GraphicsBackend& backend) const override;
};

class Shaders {
public:
    Shaders(Instance& instance, GraphicsBackend& backend);
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
    BaseProgram overlay();
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

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
