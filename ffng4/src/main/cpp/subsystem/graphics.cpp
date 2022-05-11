#include "graphics.h"
#include "game/screen.h"
#include "game/screens.h"
#include <android/bitmap.h>

void Graphics::activate() {
    LOGD("graphics: activate");
    m_system = std::make_unique<GraphicsSystem>(m_instance);
    m_system->setWindowSize(m_windowWidth, m_windowHeight);
}

void Graphics::shutdown() {
    LOGD("graphics: shutdown");
    m_system.reset();
}

void Graphics::setWindowSize(unsigned int width, unsigned int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    if(m_system)
        m_system->setWindowSize(m_windowWidth, m_windowHeight);
}

void Graphics::setMask(const Image& image) {
    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, image.texture());
    glActiveTexture(Shaders::texImage_gl);
}

void Graphics::drawFrame() {
    if(!m_system) {
        LOGE("drawFrame called without active graphics subsystem");
        return;
    }

    if(m_instance.screens().options()) {
        const auto& blur1 = m_system->m_offscreenTarget[GraphicsSystem::Targets::Blur1];
        const auto& blur2 = m_system->m_offscreenTarget[GraphicsSystem::Targets::Blur2];
        const auto& blurProgram = shaders().blur;

        blur1.bind();
        m_instance.curScreen().draw(blur1);

        glUseProgram(blurProgram);

        blur2.bind();
        glUniform2f(blurProgram.uniform("uDelta"), 1.f, 0.f);
        blur2.blitFlip(blur1.texture(), shaders().blur, false, true);

        displayTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        glUniform2f(blurProgram.uniform("uDelta"), 0.f, 1.f);
        displayTarget().blitFlip(blur2.texture(), shaders().blur, false, true);
    } else {
        displayTarget().bind();
        glClear(GL_COLOR_BUFFER_BIT);
        m_instance.curScreen().draw(displayTarget());
    }

    system().display().swap();
}

ogl::Texture Graphics::loadImage(const std::string& filename) const {
    LOGD("loadImage %s", filename.c_str());
    auto& jni = m_instance.jni();
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
    auto ret = ogl::Texture::fromImageData(system().ref(), width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;
}