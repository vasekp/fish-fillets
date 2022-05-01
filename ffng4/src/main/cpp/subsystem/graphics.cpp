#include "graphics.h"
#include <android/bitmap.h>

void Graphics::activate() {
    LOGD("graphics: activate");
    m_system = std::make_unique<GraphicsSystem>(m_instance);
}

void Graphics::shutdown() {
    LOGD("graphics: shutdown");
    m_system.reset();
}

void Graphics::setCanvasSize(unsigned width, unsigned height) {
    m_system->m_canvas.resize(width, height);
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
    std::size_t size = height * stride;
    void* pixels;
    std::unique_ptr<std::byte[]> data(new std::byte[size]);
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    auto ret = ogl::Texture::fromImageData(width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;
}