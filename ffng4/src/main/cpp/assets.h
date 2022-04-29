#ifndef FISH_FILLETS_ASSETS_H
#define FISH_FILLETS_ASSETS_H

#include "common.h"
#include "files.h"
#include "ndk.h"
#include <android/bitmap.h>
#include <android/imagedecoder.h>

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
        return {width, height, stride, std::move(data)};
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
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        memcpy(data.get(), pixels, size);
        AndroidBitmap_unlockPixels(jni, jBitmap);
        jni->DeleteLocalRef(clazz);
        jni->DeleteLocalRef(jPath);
        jni->DeleteLocalRef(jBitmap);
        return {width, height, stride, std::move(data)};
#endif
}

#endif //FISH_FILLETS_ASSETS_H
