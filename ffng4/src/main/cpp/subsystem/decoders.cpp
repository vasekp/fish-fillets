#include "decoders.h"
#include "platform/ogl.h"

#include <android/bitmap.h>

ogl::Texture Decoders::loadImage(const std::string& filename) const {
    LOGD("loadImage %s", filename.c_str());
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