#ifndef FISH_FILLETS_GRAPHICS_SHADERS_H
#define FISH_FILLETS_GRAPHICS_SHADERS_H

class Shaders {
private:
    ogl::Program m_copy;
    ogl::Program m_alpha;
    ogl::Program m_maskCopy;
    ogl::Program m_reverse;
    ogl::Program m_flat;
    ogl::Program m_blur;
    ogl::Program m_disintegrate;
    ogl::Program m_mirror;
    ogl::Program m_wavyImage;
    ogl::Program m_wavyText;
    ogl::Program m_titleText;
    ogl::Program m_zx;
    ogl::Program m_ycbcr;
    ogl::Program m_button;

public:
    ogl::Program m_arrow; // TODO private

    constexpr static GLint texImage_shader = 0;
    constexpr static GLint texMask_shader = 1;
    constexpr static GLint texCb_shader = 1;
    constexpr static GLint texCr_shader = 2;
    constexpr static GLint texImage_gl = GL_TEXTURE0;
    constexpr static GLint texMask_gl = GL_TEXTURE1;
    constexpr static GLint texCb_gl = GL_TEXTURE1;
    constexpr static GLint texCr_gl = GL_TEXTURE2;

    Shaders(const std::shared_ptr<ogl::Display>& ref, Instance& instance);

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
        FCoords srcSizeScaled; // TODO just srcSize?
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

    BaseProgram copy() { return {m_copy}; }
    Program<MaskCopyParams> maskCopy(MaskCopyParams params) { return {m_maskCopy, params}; }
    BaseProgram reverse() { return {m_reverse}; }
    Program<AlphaParams> alpha(AlphaParams params) { return {m_alpha, params}; }
    Program<FlatParams> flat(FlatParams params) { return {m_flat, params}; }
    Program<BlurParams> blur(BlurParams params) { return {m_blur, params}; }
    Program<DisintegrateParams> disintegrate(DisintegrateParams params) { return {m_disintegrate, params}; }
    BaseProgram mirror() { return {m_mirror}; }
    Program<WavyImageParams> wavyImage(WavyImageParams params) { return {m_wavyImage, params}; }
    Program<WavyTextParams> wavyText(WavyTextParams params) { return {m_wavyText, params}; }
    Program<TitleTextParams> titleText(TitleTextParams params) { return {m_titleText, params}; }
    Program<ZXParams> ZX(ZXParams params) { return {m_zx, params}; }
    BaseProgram YCbCr() { return {m_ycbcr}; } // TODO
    Program<ButtonParams> button(ButtonParams params) { return {m_button, params}; }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
