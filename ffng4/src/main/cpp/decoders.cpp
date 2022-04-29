#include "decoders.h"
#include "ogl.h"
#include "shaders.h"

#include <android/bitmap.h>

Decoders::Decoders(android_app *app_) :
    app(app_),
    jni(app->activity->vm),
    clazz(jni->GetObjectClass(app->activity->clazz))
{
    methods["loadBitmap"] = jni->GetMethodID(clazz, "loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");

    for(const auto& [key, val] : methods)
        if(!val)
            ::error("Method "s + key + " not found");
}

Decoders::~Decoders() {
    jni->DeleteLocalRef(clazz);
}

ogl::Texture Decoders::loadImage(const std::string& filename) const {
    jstring jPath = jni->NewStringUTF(filename.c_str());
    jobject jBitmap = jni->CallObjectMethod(app->activity->clazz, methods.at("loadBitmap"), jPath);
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