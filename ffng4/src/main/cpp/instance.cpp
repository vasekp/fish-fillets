#include "instance.h"
#include "ogl.h"
#include "shaders.h"

#if __ANDROID_API__ >= 30
#include <android/imagedecoder.h>
#else
#include <android/bitmap.h>
#endif

ogl::Image Instance::loadImage(const std::string& filename) const {
#if __ANDROID_API__ >= 30
    AImageDecoder* decoder;
    auto asset = systemFile(filename).asset();
    assert(AImageDecoder_createFromAAsset(asset, &decoder) == ANDROID_IMAGE_DECODER_SUCCESS);
    const AImageDecoderHeaderInfo* info = AImageDecoder_getHeaderInfo(decoder);
    std::uint32_t width = AImageDecoderHeaderInfo_getWidth(info);
    std::uint32_t height = AImageDecoderHeaderInfo_getHeight(info);
    std::size_t _stride = AImageDecoder_getMinimumStride(decoder);
    std::size_t size = height * _stride;
    std::unique_ptr<std::byte[]> data(new std::byte[size]);
    assert(AImageDecoder_decodeImage(decoder, data.get(), _stride, size) == ANDROID_IMAGE_DECODER_SUCCESS);
    AImageDecoder_delete(decoder);
    return {width, height, _stride, data.get()};
#else
    jclass clazz = jni->GetObjectClass(app->activity->clazz);
    jmethodID mid = jni->GetMethodID(clazz, "loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;"); // TODO cache
    assert(mid);
    jstring jPath = jni->NewStringUTF(filename.c_str());
    jobject jBitmap = jni->CallObjectMethod(app->activity->clazz, mid, jPath);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    std::size_t size = height * stride;
    void* pixels;
    std::unique_ptr<std::byte[]> data(new std::byte[size]);
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    ogl::Image ret{width, height, stride, pixels};
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(clazz);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;
#endif
}