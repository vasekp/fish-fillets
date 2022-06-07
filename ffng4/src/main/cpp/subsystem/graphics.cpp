#include "graphics.h"
#include "game/screens/screen.h"
#include "game/screens/screenmanager.h"
//#include <android/bitmap.h> // XXX

void Graphics::activate() {
    Log::debug("graphics: activate");
    m_system = std::make_unique<GraphicsSystem>(m_instance);
}

void Graphics::shutdown() {
    Log::debug("graphics: shutdown");
    m_system.reset();
}

void Graphics::setWindowSize(unsigned int width, unsigned int height, FCoords reserve) {
    if(!m_system)
        Log::fatal("setWindowSize() called before activate()");
    m_system->setWindowSize(width, height, reserve);
}

void Graphics::setMask(const ogl::Texture& texture) {
    glActiveTexture(Shaders::texMask_gl);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glActiveTexture(Shaders::texImage_gl);
}

void Graphics::setMask(const Image* image) {
    setMask(image->texture());
}

ogl::Texture Graphics::loadImage(const SystemFile& file) const {
    return ogl::Texture::empty(system().ref(), 1, 1);
    /*Log::debug("loadImage ", file.getPath()); // XXX
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
        Log::fatal("bitmap data null (", file.getPath(), ")");
    auto ret = ogl::Texture::fromImageData(system().ref(), width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;*/
}

ogl::Texture Graphics::renderText(const std::string& text, const std::string& font, float fontSize, float outline) const {
    return ogl::Texture::empty(system().ref(), 1, 1);
    /*auto& jni = m_instance.jni(); // XXX
    auto jFilename = jni->NewStringUTF(font.c_str());
    auto jText = jni->NewStringUTF(text.c_str());
    auto jBitmap = jni->CallObjectMethod(jni.object(), jni.method("renderText"), jText, jFilename, fontSize, outline);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    void *pixels;
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    if (!jBitmap)
        Log::fatal("bitmap data null (renderText)");
    auto ret = ogl::Texture::fromImageData(system().ref(), width, height, stride, pixels);
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(jBitmap);
    jni->DeleteLocalRef(jFilename);
    jni->DeleteLocalRef(jText);
    return ret;*/
}

float Graphics::dpi() const {
    //return (float) AConfiguration_getDensity(m_instance.app()->config); // XXX
    return 72;
}
