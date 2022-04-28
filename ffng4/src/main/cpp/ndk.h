#ifndef FISH_FILLETS_NDK_H
#define FISH_FILLETS_NDK_H

#include <android/asset_manager_jni.h>
#include <android/imagedecoder.h>

struct Image {
    std::int32_t width;
    std::int32_t height;
    std::size_t stride;
    std::unique_ptr<std::byte[]> data;
};

namespace ndk {
    class Asset {
        AAsset* asset;

    public:
        Asset() : asset(nullptr) { }

        Asset(AAssetManager *assets, const char* filename, int mode) :
                asset(AAssetManager_open(assets, filename, mode))
        {}

        Asset(const Asset&) = delete;
        Asset& operator=(const Asset&) = delete;

        Asset(Asset&& other) : asset(other.asset) {
            other.asset = nullptr;
        }

        Asset& operator=(Asset&& other) {
            std::swap(asset, other.asset);
            return *this;
        }

        ~Asset() {
            if(asset != nullptr)
                AAsset_close(asset);
        }

        bool valid() const {
            return asset != nullptr;
        }

        AAsset* operator *() {
            return asset;
        }
    };

    class ImageDecoder {
        Asset sourceRef;
        AImageDecoder* decoder;

    public:
        ImageDecoder(Asset& asset) {
            assert(AImageDecoder_createFromAAsset(*asset, &decoder) == ANDROID_IMAGE_DECODER_SUCCESS);
        }

        ImageDecoder(Asset&& asset) {
            sourceRef = std::move(asset);
            assert(AImageDecoder_createFromAAsset(*sourceRef, &decoder) == ANDROID_IMAGE_DECODER_SUCCESS);
        }

        ~ImageDecoder() {
            AImageDecoder_delete(decoder);
        }

        const Image decode() {
            const AImageDecoderHeaderInfo* info = AImageDecoder_getHeaderInfo(decoder);
            std::int32_t width = AImageDecoderHeaderInfo_getWidth(info);
            std::int32_t height = AImageDecoderHeaderInfo_getHeight(info);
            std::size_t stride = AImageDecoder_getMinimumStride(decoder);
            std::size_t size = height * stride;
            std::unique_ptr<std::byte[]> data(new std::byte[size]);
            assert(AImageDecoder_decodeImage(decoder, data.get(), stride, size) == ANDROID_IMAGE_DECODER_SUCCESS);
            return {width, height, stride, std::move(data)};
        }
    };
}

#endif //FISH_FILLETS_NDK_H
