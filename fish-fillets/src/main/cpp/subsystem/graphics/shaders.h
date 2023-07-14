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

    class Alpha : public Program {
        float m_alpha;

    public:
        struct Params {
            float alpha = 1.f;
        };

        Alpha(ogl::Program& program, Params params) :
            Program(program),
            m_alpha(params.alpha)
        { }

    private:
        void own_params() const override;
    };

    class MaskCopy : public Program {
        Color m_maskColor;

    public:
        struct Params {
            Color maskColor;
        };

        MaskCopy(ogl::Program& program, Params params) :
            Program(program),
            m_maskColor(params.maskColor)
        { }

    private:
        void own_params() const override;
    };

    class Flat : public Program {
        Color m_color;
        float m_alpha;

    public:
        struct Params {
            Color color;
            float alpha = 1.f;
        };

        Flat(ogl::Program& program, Params params) :
            Program(program),
            m_color(params.color),
            m_alpha(params.alpha)
        { }

    private:
        void own_params() const override;
    };

    class Blur : public Program {
        FCoords m_dir;

    public:
        struct Params {
            FCoords dir;
        };

        Blur(ogl::Program& program, Params params) :
            Program(program),
            m_dir(params.dir)
        { }

    private:
        void own_params() const override;
    };

    class Disintegrate : public Program {
        float m_time;

    public:
        struct Params {
            float time;
        };

        Disintegrate(ogl::Program& program, Params params) :
            Program(program),
            m_time(params.time)
        { }

    private:
        void own_params() const override;
    };

    class WavyImage : public Program {
        float m_amplitude;
        float m_period;
        float m_speed;
        float m_phase;

    public:
        struct Params {
            float amplitude;
            float period;
            float speed;
            float phase;
        };

        WavyImage(ogl::Program& program, Params params) :
            Program(program),
            m_amplitude(params.amplitude),
            m_period(params.period),
            m_speed(params.speed),
            m_phase(params.phase)
        { }

    private:
        void own_params() const override;
    };

    class WavyText : public Program {
        Color m_color1;
        Color m_color2;
        float m_time;

    public:
        struct Params {
            Color color1;
            Color color2;
            float time;
        };

        WavyText(ogl::Program& program, Params params) :
            Program(program),
            m_color1(params.color1),
            m_color2(params.color2),
            m_time(params.time)
        { }

    private:
        void own_params() const override;
    };

    class TitleText : public Program {
    public: // TODO: color changed
        FCoords m_blitSize;
        FCoords m_srcSizeScaled;
        Color m_color;
        float m_alpha;

        struct Params {
            FCoords blitSize;
            FCoords srcSizeScaled; // TODO just srcSize?
            Color color;
            float alpha;
        };

        TitleText(ogl::Program& program, Params params) :
            Program(program),
            m_blitSize(params.blitSize),
            m_srcSizeScaled(params.srcSizeScaled),
            m_color(params.color),
            m_alpha(params.alpha)
        { }

    private:
        void own_params() const override;
    };

    class ZX : public Program {
        Color m_color1;
        Color m_color2;
        float m_period;
        float m_offset;

    public:
        struct Params {
            Color color1;
            Color color2;
            float period;
            float offset;
        };

        ZX(ogl::Program& program, Params params) :
            Program(program),
            m_color1(params.color1),
            m_color2(params.color2),
            m_period(params.period),
            m_offset(params.offset)
        { }

    private:
        void own_params() const override;
    };

    class Button : public Program {
        FCoords m_texSize;
        Color m_color;
        float m_alpha;

    public:
        struct Params {
            FCoords texSize;
            Color color;
            float alpha;
        };

        Button(ogl::Program& program, Params params) :
            Program(program),
            m_texSize(params.texSize),
            m_color(params.color),
            m_alpha(params.alpha)
        { }

    private:
        void own_params() const override;
    };

    Program copy() { return {m_copy}; }
    MaskCopy maskCopy(MaskCopy::Params params) { return {m_maskCopy, params}; }
    Program reverse() { return {m_reverse}; }
    Alpha alpha(Alpha::Params params) { return {m_alpha, params}; }
    Flat flat(Flat::Params params) { return {m_flat, params}; }
    Blur blur(Blur::Params params) { return {m_blur, params}; }
    Disintegrate disintegrate(Disintegrate::Params params) { return {m_disintegrate, params}; }
    Program mirror() { return {m_mirror}; }
    WavyImage wavyImage(WavyImage::Params params) { return {m_wavyImage, params}; }
    WavyText wavyText(WavyText::Params params) { return {m_wavyText, params}; }
    TitleText titleText(TitleText::Params params) { return {m_titleText, params}; }
    ZX ZX(ZX::Params params) { return {m_zx, params}; }
    Program YCbCr() { return {m_ycbcr}; } // TODO
    Button button(Button::Params params) { return {m_button, params}; }
};

#endif //FISH_FILLETS_GRAPHICS_SHADERS_H
