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

    m_system->m_display.bind();
    glClear(GL_COLOR_BUFFER_BIT);

    m_system->m_canvas.bind();
    m_instance->curScreen()->draw();

    m_system->m_display.swap();
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