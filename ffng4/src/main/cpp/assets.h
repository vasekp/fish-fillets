#ifndef FISH_FILLETS_ASSETS_H
#define FISH_FILLETS_ASSETS_H

#include "common.h"
#include "files.h"
#include "ndk.h"

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#if __ANDROID_API__ >= 30
#include <android/imagedecoder.h>
#else
#include <android/bitmap.h>
#endif

class Texture {
    GLuint texture;

public:
    Texture() {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    ~Texture() {
        glDeleteTextures(1, &texture);
    }

    operator GLuint() const {
        return texture;
    }
};

struct Image {
    const std::uint32_t width;
    const std::uint32_t height;
    const std::size_t stride;
    const Texture texture;

    Image(const uint32_t width, const uint32_t height, const size_t stride, void* data) :
        width(width), height(height), stride(stride), texture()
    {
        assert(stride == 4 * width); // GLES does not support GL_UNPACK_ROW_LENGTH
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    GLuint texId() const {
        return texture;
    }
};

Image loadImage(const SystemFile& filename, Instance& instance) {
#if __ANDROID_API__ >= 30
    AImageDecoder* decoder;
    auto asset = filename.asset();
    assert(AImageDecoder_createFromAAsset(asset, &decoder) == ANDROID_IMAGE_DECODER_SUCCESS);
    const AImageDecoderHeaderInfo* info = AImageDecoder_getHeaderInfo(decoder);
    std::uint32_t width = AImageDecoderHeaderInfo_getWidth(info);
    std::uint32_t height = AImageDecoderHeaderInfo_getHeight(info);
    std::size_t stride = AImageDecoder_getMinimumStride(decoder);
    std::size_t size = height * stride;
    std::unique_ptr<std::byte[]> data(new std::byte[size]);
    assert(AImageDecoder_decodeImage(decoder, data.get(), stride, size) == ANDROID_IMAGE_DECODER_SUCCESS);
    AImageDecoder_delete(decoder);
    return {width, height, stride, data.get()};
#else
    const auto& jni = instance.jni;
    jclass clazz = jni->GetObjectClass(instance.app->activity->clazz);
    jmethodID mid = jni->GetMethodID(clazz, "loadBitmap", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    assert(mid);
    jstring jPath = jni->NewStringUTF(filename.getPath().c_str());
    jobject jBitmap = jni->CallObjectMethod(instance.app->activity->clazz, mid, jPath);
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(jni, jBitmap, &info);
    std::uint32_t width = info.width;
    std::uint32_t height = info.height;
    std::size_t stride = info.stride;
    std::size_t size = height * stride;
    void* pixels;
    std::unique_ptr<std::byte[]> data(new std::byte[size]);
    AndroidBitmap_lockPixels(jni, jBitmap, &pixels);
    Image ret{width, height, stride, pixels};
    AndroidBitmap_unlockPixels(jni, jBitmap);
    jni->DeleteLocalRef(clazz);
    jni->DeleteLocalRef(jPath);
    jni->DeleteLocalRef(jBitmap);
    return ret;
#endif
}

#endif //FISH_FILLETS_ASSETS_H
