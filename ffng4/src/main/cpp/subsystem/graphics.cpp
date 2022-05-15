#include "graphics.h"
#include "game/screens/screen.h"
#include "game/screens/screenmanager.h"
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(Shaders::texImage_gl);
}

ogl::Texture Graphics::loadImage(const SystemFile& file) const {
    LOGD("loadImage %s", file.getPath().c_str());
    auto& jni = m_instance.jni();
    jstring jPath = jni->NewStringUTF(file.getPath().c_str());
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

ogl::Texture Graphics::renderText(const std::string& text, const std::string& font, float fontSize, float outline) const {
    auto& jni = m_instance.jni();
    auto jFilename = jni->NewStringUTF(font.c_str());
    auto jText = jni->NewStringUTF(text.c_str());
    auto jBitmap = jni->CallObjectMethod(jni.object(), jni.method("renderText"), jText, jFilename, fontSize, outline, m_instance.graphics().display().width());
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    void *pixels;
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    if (!jBitmap)
        throw std::runtime_error("bitmap data null");
    auto ret = ogl::Texture::fromImageData(system().ref(), width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jBitmap);
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    return ret;
}