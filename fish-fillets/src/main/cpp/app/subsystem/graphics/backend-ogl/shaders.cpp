#include "../graphicsbackend.h"
#include "subsystem/graphics.h"
#include "subsystem/files.h"

struct Shaders::Impl {
    BACKEND::Program copy;
    BACKEND::Program overlay;
    BACKEND::Program maskCopy;
    BACKEND::Program alpha;
    BACKEND::Program reverse;
    BACKEND::Program mirror;
    BACKEND::Program flat;
    BACKEND::Program blur;
    BACKEND::Program disintegrate;
    BACKEND::Program wavyImage;
    BACKEND::Program wavyText;
    BACKEND::Program titleText;
    BACKEND::Program zx;
    BACKEND::Program ycbcr;
    BACKEND::Program button;
    BACKEND::Program arrow;
};

Shaders::~Shaders() = default;

// NB. we can't get backend from instance.graphics().backend() yet because this function is called when Instance is being constructed.
Shaders::Shaders(Instance& instance, GraphicsBackend& backend) {
    auto& display = backend.display();
    auto& files = instance.files();
    auto shader = [&](std::string filename, GLenum type) {
        return ogl::Shader{display, type, files.system("shader/opengl/"s + filename)->read()};
    };
    auto vert = [&](std::string filename) { return shader(filename, GL_VERTEX_SHADER); };
    auto frag = [&](std::string filename) { return shader(filename, GL_FRAGMENT_SHADER); };
    ogl::Shader pixelVert = vert("pixel.vert");
    pImpl = std::make_unique<Impl>(Impl{
            ogl::Program{display, pixelVert, frag("copy.frag")},
            ogl::Program{display, pixelVert, frag("overlay.frag")},
            ogl::Program{display, pixelVert, frag("mask-copy.frag")},
            ogl::Program{display, pixelVert, frag("alpha.frag")},
            ogl::Program{display, pixelVert, frag("reverse.frag")},
            ogl::Program{display, pixelVert, frag("mirror.frag")},
            ogl::Program{display, pixelVert, frag("flat.frag")},
            ogl::Program{display, pixelVert, frag("blur.frag")},
            ogl::Program{display, pixelVert, frag("disintegrate.frag")},
            ogl::Program{display, pixelVert, frag("wavy-image.frag")},
            ogl::Program{display, pixelVert, frag("wavy-text.frag")},
            ogl::Program{display, pixelVert, frag("title.frag")},
            ogl::Program{display, pixelVert, frag("zx.frag")},
            ogl::Program{display, pixelVert, frag("ycbcr.frag")},
            ogl::Program{display, pixelVert, frag("button.frag")},
            ogl::Program{display, vert("arrow.vert"), frag("arrow.frag")}
    });

    for(const auto* program : {&pImpl->copy, &pImpl->overlay, &pImpl->maskCopy,
            &pImpl->alpha, &pImpl->reverse, &pImpl->mirror, &pImpl->blur, &pImpl->disintegrate,
            &pImpl->wavyImage, &pImpl->wavyText, &pImpl->titleText, &pImpl->zx, &pImpl->button}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uSrcTexture"), 0);
    }

    for(const auto* program : {&pImpl->maskCopy, &pImpl->mirror}) {
        glUseProgram(*program);
        glUniform1i(program->uniform("uMaskTexture"), 1);
    }

    glUseProgram(pImpl->ycbcr);
    glUniform1i(pImpl->ycbcr.uniform("uCbTexture"), 1);
    glUniform1i(pImpl->ycbcr.uniform("uCrTexture"), 2);
}

void BaseProgram::run(GraphicsBackend& backend, DrawTarget& target, const BaseProgram::Params& params, Shape shape,
    const BaseProgram::Textures& textures) const {
    int texIndex = 0;
    for(auto ref : textures) {
        glActiveTexture(GL_TEXTURE0 + texIndex);
        glBindTexture(GL_TEXTURE_2D, ref.get().native());
        texIndex++;
    }
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(m_native);
    glUniform2fv(m_native.uniform("uSrcSize"), 1, params.srcSize.data());
    glUniform2fv(m_native.uniform("uDstSize"), 1, params.dstSize.data());
    glUniform2fv(m_native.uniform("uSrcOffset"), 1, params.src.data());
    glUniform2fv(m_native.uniform("uDstOffset"), 1, params.dest.data());
    glUniform2fv(m_native.uniform("uArea"), 1, params.area.data());
    glUniform4fv(m_native.uniform("uCoords"), 1, params.coords.data());

    own_params(backend);

    static constexpr float rect[4][2] = { {0, 0}, {0, 1}, {1, 0}, {1, 1} };
    static constexpr float triangle[3][3] = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };

    switch(shape) {
        case Shape::rect: {
            glVertexAttribPointer(ogl::Program::aPosition, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), &rect[0][0]);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            break;
        }
        case Shape::triangle: {
            glVertexAttribPointer(ogl::Program::aPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), &triangle[0][0]);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            break;
        }
    }
}

template<>
void Program<Shaders::AlphaParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform1f(m_native.uniform("uAlpha"), m_params.alpha);
}

template<>
void Program<Shaders::MaskCopyParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uMaskColor"), 1, m_params.maskColor.data());
}

template<>
void Program<Shaders::FlatParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::BlurParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform2fv(m_native.uniform("uDelta"), 1, m_params.dir.data());
}

template<>
void Program<Shaders::DisintegrateParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::WavyImageParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform1f(m_native.uniform("uAmplitude"), m_params.amplitude);
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uPhase"), m_params.phase);
}

template<>
void Program<Shaders::WavyTextParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.data());
    glUniform1f(m_native.uniform("uTime"), m_params.time);
}

template<>
void Program<Shaders::TitleTextParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::ZXParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uColor1"), 1, m_params.color1.data());
    glUniform4fv(m_native.uniform("uColor2"), 1, m_params.color2.data());
    glUniform1f(m_native.uniform("uPeriod"), m_params.period);
    glUniform1f(m_native.uniform("uOffset"), m_params.offset);
}

template<>
void Program<Shaders::ButtonParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

template<>
void Program<Shaders::ArrowParams>::own_params([[maybe_unused]] GraphicsBackend& backend) const {
    glUniform2fv(m_native.uniform("uPosition"), 1, m_params.position.data());
    glUniform2fv(m_native.uniform("uDirection"), 1, m_params.direction.data());
    glUniform1f(m_native.uniform("uSize"), m_params.size);
    glUniform1f(m_native.uniform("uSign"), m_params.sign);
    glUniform4fv(m_native.uniform("uColor"), 1, m_params.color.data());
}

BaseProgram Shaders::copy() { return {pImpl->copy}; }

BaseProgram Shaders::overlay() { return {pImpl->overlay}; }

Program<Shaders::MaskCopyParams> Shaders::maskCopy(MaskCopyParams params) { return {pImpl->maskCopy, params}; }

BaseProgram Shaders::reverse() { return {pImpl->reverse}; }

BaseProgram Shaders::mirror() { return {pImpl->mirror}; }

Program<Shaders::AlphaParams> Shaders::alpha(AlphaParams params) { return {pImpl->alpha, params}; }

Program<Shaders::FlatParams> Shaders::flat(FlatParams params) { return {pImpl->flat, params}; }

Program<Shaders::BlurParams> Shaders::blur(BlurParams params) { return {pImpl->blur, params}; }

Program<Shaders::DisintegrateParams> Shaders::disintegrate(DisintegrateParams params) { return {pImpl->disintegrate, params}; }

Program<Shaders::WavyImageParams> Shaders::wavyImage(WavyImageParams params) { return {pImpl->wavyImage, params}; }

Program<Shaders::WavyTextParams> Shaders::wavyText(WavyTextParams params) { return {pImpl->wavyText, params}; }

Program<Shaders::TitleTextParams> Shaders::titleText(TitleTextParams params) { return {pImpl->titleText, params}; }

Program<Shaders::ZXParams> Shaders::ZX(ZXParams params) { return {pImpl->zx, params}; }

BaseProgram Shaders::YCbCr() { return {pImpl->ycbcr}; }

Program<Shaders::ButtonParams> Shaders::button(ButtonParams params) { return {pImpl->button, params}; }

Program<Shaders::ArrowParams> Shaders::arrow(ArrowParams params) { return {pImpl->arrow, params}; }
