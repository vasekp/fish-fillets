#include "graphics.h"
#include "game/screen.h"
#include <android/bitmap.h>

void Graphics::activate() {
    LOGD("graphics: activate");
    m_system = std::make_shared<GraphicsSystem>(m_instance);
}

void Graphics::shutdown() {
    LOGD("graphics: shutdown");
    m_system.reset();
}

void Graphics::setCanvasSize(unsigned width, unsigned height) {
    m_system->m_canvas.resize(width, height);
    auto& program = m_instance->graphics->shaders()->fill;
    glUseProgram(program);
    auto fsw = (float)m_system->m_canvas.width();
    auto fsh = (float)m_system->m_canvas.height();
    auto fdw = (float)m_system->m_display.width();
    auto fdh = (float)m_system->m_display.height();
    glUniform2f(program.uniform("uDisplaySize"), fdw, fdh);
    glUniform2f(program.uniform("uCanvasSize"), fsw, fsh);

    float scale = std::min(fdw / fsw, fdh / fsh);
    viewport = {(int)(fdw - scale * fsw) / 2,
                (int)(fdh - scale * fsh) / 2,
                (int)(scale * fsw), (int)(scale * fsh)};

    glViewport(viewport.vx, viewport.vy, viewport.vw, viewport.vh);
}

void Graphics::setMask(const Image* image) {
    m_system->m_maskBuffer.bindWith(image->texture());

    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, image->texture());
    glActiveTexture(Shaders::texImage_gl);
}

void Graphics::drawFrame() {
    if(!m_system) {
        LOGE("drawFrame called without active graphics subsystem");
        return;
    }

    m_system->m_canvas.bind();
    m_instance->curScreen()->draw();

    m_system->m_display.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    glViewport(viewport.vx, viewport.vy, viewport.vw, viewport.vh);
    glUseProgram(m_instance->graphics->shaders()->fill);
    GraphicsUtils::rect(-1.f, -1.f, 2.f, 2.f);

    m_system->m_display.swap();
}

std::pair<int, int> Graphics::screen2canvas(int x, int y) {
    return {
    (int)((float) (x - viewport.vx) / (float) viewport.vw * (float) m_system->m_canvas.width()),
    (int)((float) (y - viewport.vy) / (float) viewport.vh * (float) m_system->m_canvas.height())
    };
}

std::pair<int, int> Graphics::canvas2screen(int x, int y) {
    return {
    (int)((float) x / (float) m_system->m_canvas.width() * (float) viewport.vw) + viewport.vx,
    (int)((float) y / (float) m_system->m_canvas.height() * (float) viewport.vh) + viewport.vy
    };
}

ogl::Texture Graphics::loadImage(const std::string& filename) const {
    LOGD("loadImage %s", filename.c_str());
    auto& jni = m_instance->jni;
    jstring jPath = jni->NewStringUTF(filename.c_str());
    jobject jBitmap = jni->CallObjectMethod(jni.object(), jni.method("loadBitmap"), jPath);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    void* pixels;
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    if(!jBitmap)
        throw std::runtime_error("bitmap data null");
    auto ret = ogl::Texture::fromImageData(width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;
}